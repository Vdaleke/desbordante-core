#include "non_fd_tree.h"

#include <memory>
#include <vector>

#include <boost/dynamic_bitset.hpp>

namespace algos::dynfd {

std::shared_ptr<NonFDTreeVertex> NonFDTree::AddNonFD(
        boost::dynamic_bitset<> const& lhs, size_t rhs,
        ViolatingRecordPair violationPair = std::nullopt) {
    NonFDTreeVertex* cur_node = root_.get();
    cur_node->SetAttribute(rhs);

    for (size_t bit = lhs.find_first(); bit != boost::dynamic_bitset<>::npos;
         bit = lhs.find_next(bit)) {
        bool is_new = cur_node->AddChild(bit);

        if (is_new && lhs.find_next(bit) == boost::dynamic_bitset<>::npos) {
            auto added_node = cur_node->GetChildPtr(bit);
            added_node->SetAttribute(rhs);
            added_node->SetNonFd(rhs, violationPair);
            return added_node;
        }

        cur_node = cur_node->GetChild(bit);
        cur_node->SetAttribute(rhs);
    }
    cur_node->SetNonFd(rhs, violationPair);
    return nullptr;
}

bool NonFDTree::ContainsNonFD(boost::dynamic_bitset<>& lhs, size_t rhs) {
    NonFDTreeVertex const* cur_node = root_.get();

    for (size_t bit = lhs.find_first(); bit != boost::dynamic_bitset<>::npos;
         bit = lhs.find_next(bit)) {
        if (!cur_node->HasChildren() || !cur_node->ContainsChildAt(bit)) {
            return false;
        }

        cur_node = cur_node->GetChild(bit);
    }

    return cur_node->IsNonFd(rhs);
}

std::shared_ptr<NonFDTreeVertex> NonFDTree::FindNonFdVertex(boost::dynamic_bitset<> const& lhs) {
    auto cur_node = root_;

    for (size_t bit = lhs.find_first(); bit != boost::dynamic_bitset<>::npos;
         bit = lhs.find_next(bit)) {
        if (!cur_node->HasChildren() || !cur_node->ContainsChildAt(bit)) {
            return nullptr;
        }

        cur_node = cur_node->GetChildShared(bit);
    }

    return cur_node;
}

std::vector<boost::dynamic_bitset<>> NonFDTree::GetNonFdAndGenerals(boost::dynamic_bitset<>& lhs,
                                                                    size_t rhs) const {
    assert(lhs.count() != 0);

    std::vector<boost::dynamic_bitset<>> result;
    boost::dynamic_bitset const empty_lhs(GetNumAttributes());
    size_t const starting_bit = lhs.find_first();

    root_->GetNonFdAndGeneralsRecursive(lhs, empty_lhs, rhs, starting_bit, result);

    return result;
}

std::vector<boost::dynamic_bitset<>> NonFDTree::GetGenerals(boost::dynamic_bitset<>& lhs,
                                                            size_t rhs) {
    assert(lhs.count() != 0);

    std::vector<boost::dynamic_bitset<>> result;
    boost::dynamic_bitset empty_lhs(GetNumAttributes());
    size_t const starting_bit = lhs.find_first();

    root_->GetGeneralsRecursive(lhs, empty_lhs, rhs, starting_bit, result);

    return result;
}

std::vector<boost::dynamic_bitset<>> NonFDTree::GetNonFdAndSpecials(boost::dynamic_bitset<>& lhs,
                                                                    size_t rhs) {
    std::vector<boost::dynamic_bitset<>> result;
    boost::dynamic_bitset empty_lhs(GetNumAttributes());

    root_->GetNonFdAndSpecialsRecursive(lhs, empty_lhs, rhs, 0, result);

    return result;
}

void NonFDTree::RemoveGenerals(boost::dynamic_bitset<> const& lhs, size_t rhs) {
    assert(lhs.count() != 0);

    boost::dynamic_bitset<> empty_lhs(GetNumAttributes());
    root_->RemoveGeneralsRecursive(lhs, empty_lhs, rhs, lhs.find_first());
}

std::vector<boost::dynamic_bitset<>> NonFDTree::GetSpecials(boost::dynamic_bitset<>& lhs,
                                                            size_t rhs) {
    std::vector<boost::dynamic_bitset<>> result;
    boost::dynamic_bitset empty_lhs(GetNumAttributes());

    root_->GetSpecialsRecursive(lhs, empty_lhs, rhs, 0, result);

    return result;
}

void NonFDTree::RemoveSpecials(boost::dynamic_bitset<>& lhs, size_t rhs) {
    boost::dynamic_bitset empty_lhs(GetNumAttributes());
    root_->RemoveSpecialsRecursive(lhs, empty_lhs, rhs, 0);
}

bool NonFDTree::ContainsNonFdOrSpecial(boost::dynamic_bitset<>& lhs, size_t rhs) const {
    size_t next_after_last_lhs_set_bit = 0;
    if (lhs.find_first() != boost::dynamic_bitset<>::npos) {
        next_after_last_lhs_set_bit = lhs.find_first();
        while (lhs.find_next(next_after_last_lhs_set_bit) != boost::dynamic_bitset<>::npos) {
            next_after_last_lhs_set_bit = lhs.find_next(next_after_last_lhs_set_bit);
        }
        ++next_after_last_lhs_set_bit;
    }

    return root_->ContainsNonFdOrSpecialRecursive(lhs, rhs, next_after_last_lhs_set_bit, 0);
}

std::vector<LhsPair> NonFDTree::GetLevel(unsigned int target_level) {
    boost::dynamic_bitset const empty_lhs(GetNumAttributes());

    std::vector<LhsPair> vertices;
    root_->GetLevelRecursive(target_level, 0, empty_lhs, vertices);
    return vertices;
}

std::vector<RawFD> NonFDTree::FillNonFDs() const {
    std::vector<RawFD> result;
    boost::dynamic_bitset<> lhs_for_traverse(GetRoot().GetNumAttributes());
    GetRoot().FillNonFDs(result, lhs_for_traverse);
    return result;
}

}  // namespace algos::dynfd