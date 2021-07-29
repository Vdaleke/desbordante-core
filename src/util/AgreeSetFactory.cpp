#include "AgreeSetFactory.h"

#include <unordered_set>

#include "IdentifierSet.h"
#include "logging/easylogging++.h"

using std::set, std::vector, std::unordered_set;

AgreeSetFactory::SetOfAgreeSets AgreeSetFactory::genAgreeSets() const {
    auto start_time = std::chrono::system_clock::now();
    std::string method_str;
    SetOfAgreeSets agree_sets;

    switch (config_.as_gen_method) {
      case AgreeSetsGenMethod::kUsingVectorOfIDSets: {
        method_str = "`kUsingVectorOfIDSets`";
        agree_sets = genASUsingVectorOfIDSets();
        break;
      }
      case AgreeSetsGenMethod::kUsingMapOfIDSets: {
        method_str = "`kUsingMapOfIDSets`";
        agree_sets = genASUsingMapOfIDSets();
        break;
      }
      case AgreeSetsGenMethod::kUsingMCAndGetAgreeSet: {
        method_str = "`kUsingMCAndGetAgreeSet`";
        agree_sets = genASUsingMapOfIDSets();
        break;
      }
      case AgreeSetsGenMethod::kUsingGetAgreeSet: {
        method_str = "`kUsingGetAgreeSet`";
        agree_sets = genASUsingGetAgreeSets();
        break;
      }
    }

    // metanome kostil, doesn't work properly in general
    agree_sets.insert(*relation_->getSchema()->emptyVertical);

    auto elapsed_mills_to_gen_agree_sets =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - start_time
            );
    LOG(INFO) << "TIME TO GENERATE AGREE SETS WITH METHOD "
              << method_str << ": "
              << elapsed_mills_to_gen_agree_sets.count();

    return agree_sets;
}

AgreeSetFactory::SetOfAgreeSets AgreeSetFactory::genASUsingVectorOfIDSets() const {
    SetOfAgreeSets agree_sets;
    vector<IdentifierSet> identifier_sets;
    SetOfVectors const max_representation = genPLIMaxRepresentation();

    auto start_time = std::chrono::system_clock::now();

    // compute identifier sets
    // identifier_sets is vector
    std::unordered_set<int> cache;
    for (auto const& cluster : max_representation) {
        for (auto p = cluster.begin(); p != cluster.end(); ++p) {
            if (!cache.insert(*p).second) {
                continue;
            }
            identifier_sets.emplace_back(IdentifierSet(relation_, *p));
        }
    }

    auto elapsed_mills_to_gen_id_sets =
        std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - start_time
                );
    LOG(INFO) << "TIME TO IDENTIFIER SETS GENERATION: "
        << elapsed_mills_to_gen_id_sets.count();

    LOG(DEBUG) << "Identifier sets:";
    for (auto const& id_set : identifier_sets) {
        LOG(DEBUG) << id_set.toString();
    }

    // compute agree sets using identifier sets
    // using vector of identifier sets
    if (!identifier_sets.empty()) {
        auto back_it = std::prev(identifier_sets.end());
        for (auto p = identifier_sets.begin(); p != back_it; ++p) {
            for (auto q = std::next(p); q != identifier_sets.end(); ++q) {
                agree_sets.insert(p->intersect(*q));
            }
        }
    }

    return agree_sets;
}

AgreeSetFactory::SetOfAgreeSets AgreeSetFactory::genASUsingMapOfIDSets() const {
    SetOfAgreeSets agree_sets;
    std::unordered_map<int, IdentifierSet> identifier_sets;
    SetOfVectors const max_representation = genPLIMaxRepresentation();

    auto start_time = std::chrono::system_clock::now();

    for (auto const& cluster : max_representation) {
        for (auto p = cluster.begin(); p != cluster.end(); ++p) {
            identifier_sets.emplace(*p, IdentifierSet(relation_, *p));
        }
    }

    auto elapsed_mills_to_gen_id_sets =
        std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - start_time
                );
    LOG(INFO) << "TIME TO IDENTIFIER SETS GENERATION: "
        << elapsed_mills_to_gen_id_sets.count();


    LOG(DEBUG) << "Identifier sets:";
    for (auto const& [index, id_set] : identifier_sets) {
        LOG(DEBUG) << id_set.toString();
    }


    // compute agree sets using identifier sets
    // metanome approach (using map of identifier sets)
    for (auto const &cluster : max_representation) {
        auto back_it = std::prev(cluster.end());
        for (auto p = cluster.begin(); p != back_it; ++p) {
            for (auto q = std::next(p); q != cluster.end(); ++q) {
                IdentifierSet const& id_set1 = identifier_sets.at(*p);
                IdentifierSet const& id_set2 = identifier_sets.at(*q);
                agree_sets.insert(id_set1.intersect(id_set2));
            }
        }
    }

    return agree_sets;
}

AgreeSetFactory::SetOfAgreeSets AgreeSetFactory::genASUsingMCAndGetAgreeSets() const {
    SetOfAgreeSets agree_sets;
    SetOfVectors const max_representation = genPLIMaxRepresentation();

    // Compute agree sets from maximal representation using getAgreeSet()
    // ~3300 ms on CIPublicHighway700 (Debug build), ~250 ms (Release)
    for (auto const& cluster : max_representation) {
        for (auto p = cluster.begin(); p != cluster.end(); ++p) {
            for (auto q = std::next(p); q != cluster.end(); ++q) {
                agree_sets.insert(getAgreeSet(*p, *q));
            }
        }
    }

    return agree_sets;
}

AgreeSetFactory::SetOfAgreeSets AgreeSetFactory::genASUsingGetAgreeSets() const {
    SetOfAgreeSets agree_sets;
    vector<ColumnData> const& columns_data = relation_->getColumnData();

    // Compute agree sets from stripped partitions (simplest method by Wyss)
    // ~40436 ms on CIPublicHighway700 (Debug build)
    for (ColumnData const& column_data : columns_data) {
        PositionListIndex const* const pli = column_data.getPositionListIndex();
        for (vector<int> const& cluster : pli->getIndex()) {
            for (auto p = cluster.begin(); p != cluster.end(); ++p) {
                for (auto q = std::next(p); q != cluster.end(); ++q) {
                    agree_sets.insert(getAgreeSet(*p, *q));
                }
            }
        }
    }

    return agree_sets;
}

AgreeSet AgreeSetFactory::getAgreeSet(int const tuple1_index,
                                      int const tuple2_index) const {
    std::vector<int> const tuple1 = relation_->getTuple(tuple1_index);
    std::vector<int> const tuple2 = relation_->getTuple(tuple2_index);
    boost::dynamic_bitset<> agree_set_indices(relation_->getNumColumns());

    for (size_t i = 0; i < agree_set_indices.size(); ++i) {
        if (tuple1[i] != 0 && tuple1[i] == tuple2[i]) {
            agree_set_indices.set(i);
        }
    }

    return relation_->getSchema()->getVertical(agree_set_indices);
}

AgreeSetFactory::SetOfVectors AgreeSetFactory::genPLIMaxRepresentation() const {
    SetOfVectors max_representation;
    std::string method_str;
    auto start_time = std::chrono::system_clock::now();

    switch (config_.mc_gen_method) {
      case MCGenMethod::kUsingCalculateSupersets: {
        method_str = "`kUsingCalculateSupersets`";
        max_representation = genMCUsingCalculateSupersets();
        break;
      }
      case MCGenMethod::kUsingHandleEqvClass: {
        method_str = "`kUsingHandleEqvClass`";
        max_representation = genMCUsingHandleEqvClass();
        break;
      }
      case MCGenMethod::kUsingHandlePartition: {
        method_str = "`kUsingHandlePartition`";
        max_representation = genMCUsingHandlePartition();
        break;
      }
    }

    auto elapsed_mills_to_gen_max_representation =
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - start_time
        );
    LOG(INFO) << "TIME TO GENERATE MAX REPRESENTATION WITH METHOD "
              << method_str << ": "
              << elapsed_mills_to_gen_max_representation.count();

    return max_representation;
}

AgreeSetFactory::SetOfVectors AgreeSetFactory::genMCUsingCalculateSupersets() const {
    vector<ColumnData> const& columns_data = relation_->getColumnData();
    SetOfVectors max_representation;

    auto not_empty_pli =
        std::find_if(columns_data.begin(), columns_data.end(),
                     [](ColumnData const& c) {
                         return c.getPositionListIndex()->getSize() != 0;
                     });

    if (not_empty_pli == columns_data.end()) {
        return max_representation;
    }

    max_representation.insert(not_empty_pli->getPositionListIndex()->getIndex().begin(),
                              not_empty_pli->getPositionListIndex()->getIndex().end());

    for (auto p = std::next(not_empty_pli); p != columns_data.end(); ++p) {
        PositionListIndex const* pli = p->getPositionListIndex();
        if (pli->getSize() != 0) {
            calculateSupersets(max_representation, pli->getIndex());
        }
    }

    return max_representation;
}

AgreeSetFactory::SetOfVectors AgreeSetFactory::genMCUsingHandleEqvClass() const {
    vector<ColumnData> const& columns_data = relation_->getColumnData();
    SetOfVectors max_representation;
    // set of all equivalence classes of all paritions
    auto comp = [](vector<int> const& lhs, vector<int> const& rhs) {
        if (lhs.size() != rhs.size()) {
            return lhs.size() < rhs.size();
        }
        return std::lexicographical_compare(lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end());
    };
    set<vector<int>, decltype(comp)> sorted_eqv_classes(comp);

    // Fill sorted_partitions
    for (ColumnData const& data : columns_data) {
        std::deque<vector<int>> const& index = data.getPositionListIndex()->getIndex();
        sorted_eqv_classes.insert(index.begin(), index.end());
    }

    if (sorted_eqv_classes.empty()) {
        return max_representation;
    }

    // Maximize partitions
    size_t const min_size = sorted_eqv_classes.begin()->size();
    std::unordered_map<size_t, SetOfVectors> max_sets;
    SetOfVectors min_set;

    auto const first_not_min
        = std::find_if_not(sorted_eqv_classes.begin(), sorted_eqv_classes.end(),
                [min_size](auto const& p) { return p.size() == min_size; });

    min_set.insert(sorted_eqv_classes.begin(), first_not_min);
    max_sets.emplace(min_size, std::move(min_set));

    for (auto it = first_not_min; it != sorted_eqv_classes.end();) {
        // So that the eqv_class can be modified
        vector<int> eqv_class = sorted_eqv_classes.extract(it++).value();
        handleEqvClass(eqv_class, max_sets, true);
    }

    // Metanome `mergeResult`
    max_representation.reserve(max_sets.size());
    for (auto it = max_sets.begin(); it != max_sets.end();) {
        SetOfVectors set = max_sets.extract(it++).mapped();
        max_representation.insert(std::make_move_iterator(set.begin()),
                std::make_move_iterator(set.end()));
    }
    assert(max_sets.empty());

    return max_representation;
}

AgreeSetFactory::SetOfVectors AgreeSetFactory::genMCUsingHandlePartition() const {
    vector<ColumnData> const& columns_data = relation_->getColumnData();
    // set of all equivalence classes of all paritions
    auto greater = [](vector<int> const& lhs, vector<int> const& rhs) {
        if (lhs.size() != rhs.size()) {
            return lhs.size() > rhs.size();
        }
        return std::lexicographical_compare(lhs.begin(), lhs.end(),
                                            rhs.begin(), rhs.end(),
                                            std::greater<int>());
    };
    set<vector<int>, decltype(greater)> sorted_eqv_classes(greater);
    SetOfVectors max_representation;
    /* maps tuple_index to set of eqv_classes (each eqv_class represented
     * by index in sorted_eqv_classes, so set<size_t>) in which this tuple_index appears.
     * It would be possible to use decltype(sorted_eqv_classes)::const_iterator to
     * represent elements of sorted_eqv_classes, but imo this would overcomplicate the code.
     */
    std::unordered_map<int, unordered_set<size_t>> index;

    // Fill sorted_partitions
    for (ColumnData const& data : columns_data) {
        std::deque<vector<int>> const& index = data.getPositionListIndex()->getIndex();
        sorted_eqv_classes.insert(index.begin(), index.end());
    }


    size_t eqv_class_index = 0;
    for (auto it = sorted_eqv_classes.begin();
         it != sorted_eqv_classes.end();
         ++it, ++eqv_class_index) {
        // handlePartition method in Metanome
        if (!isSubset(*it, index)) {
            for (int tuple_index : *it) {
                index[tuple_index].insert(eqv_class_index);
            }
            max_representation.insert(std::move(*it));
        }
    }

    return max_representation;

}

bool AgreeSetFactory::isSubset(vector<int> const& eqv_class,
                               std::unordered_map<int, unordered_set<size_t>> const& index) const {
    unordered_set<size_t> intersection;
    auto intersect = [&intersection, &index] (int tuple_index) {
        for (auto it = intersection.begin(); it != intersection.end();) {
            auto p = it++;
            if (index.at(tuple_index).count(*p) == 0) {
                intersection.erase(*p);
            }
        }
    };

    for (auto it = eqv_class.begin(); it != eqv_class.end(); ++it) {
        if (index.count(*it) == 0) {
            return false;
        }

        if (intersection.empty()) {
            intersection.insert(index.at(*it).begin(), index.at(*it).end());
        }

        intersect(*it);

        if (intersection.empty()) {
            return false;
        }
    }

    return true;
}

void AgreeSetFactory::handleEqvClass(vector<int>& eqv_class,
                                     std::unordered_map<size_t, SetOfVectors>& max_sets,
                                     bool const first_step) const {
    for (auto it = eqv_class.begin(); it != eqv_class.end(); ++it) {
        vector<int> copy(eqv_class.begin(), it);
        copy.insert(copy.end(), std::next(it), eqv_class.end());

        size_t const size = copy.size();
        assert(size == eqv_class.size() - 1);

        /* Need to check if an element with copy.size() key exists before accessing it
         * to avoid new SetOfVectors() creation if it doesn't.
         */
        if (max_sets.count(size) != 0 &&
            max_sets[size].find(copy) != max_sets[size].end()) {
            max_sets[size].erase(copy);
        } else {
            if (size > 2) {
                handleEqvClass(copy, max_sets, false);
            }
        }
    }

    if (first_step)
        max_sets[eqv_class.size()].insert(std::move(eqv_class));
}

void AgreeSetFactory::calculateSupersets(SetOfVectors& max_representation,
                                         std::deque<vector<int>> const& partition) const {
    SetOfVectors to_add_to_mc;
    auto hash = [beg = max_representation.begin()](SetOfVectors::const_iterator it) {
        return std::distance<SetOfVectors::const_iterator>(beg, it);
    };
    unordered_set<SetOfVectors::const_iterator, decltype(hash)> to_delete_from_mc(1, hash);
    set<std::deque<vector<int>>::const_iterator> to_exclude_from_partition;

    for (auto it = max_representation.begin(); it != max_representation.end(); ++it) {
        for (auto p = partition.begin();
             to_exclude_from_partition.size() != partition.size() && p != partition.end();
             ++p) {
            if (to_exclude_from_partition.find(p) != to_exclude_from_partition.end()) {
                continue;
            }

            if (it->size() >= p->size() &&
                std::includes(it->begin(), it->end(), p->begin(), p->end())) {
                to_add_to_mc.erase(*p);
                to_exclude_from_partition.insert(p);
                break;
            }

            if (p->size() >= it->size() &&
                std::includes(p->begin(), p->end(), it->begin(), it->end())) {
                to_delete_from_mc.insert(it);
            }

            to_add_to_mc.insert(*p);
        }
    }

    for (auto& cluster : to_add_to_mc) {
        max_representation.insert(std::move(cluster));
    }
    for (auto it : to_delete_from_mc) {
        max_representation.erase(it);
    }
}
