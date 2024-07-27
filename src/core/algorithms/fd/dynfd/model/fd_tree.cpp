#include "fd_tree.h"

#include <memory>
#include <vector>

#include <boost/dynamic_bitset.hpp>

namespace model::dynfd {
std::shared_ptr<FDTreeVertex> FDTree::AddFD(boost::dynamic_bitset<> const &lhs, size_t const rhs) {
    FDTreeVertex *cur_node = root_.get();
    cur_node->SetAttribute(rhs);

    for (size_t bit = lhs.find_first(); bit != boost::dynamic_bitset<>::npos;
         bit = lhs.find_next(bit)) {
        bool is_new = cur_node->AddChild(bit);

        if (is_new && lhs.find_next(bit) == boost::dynamic_bitset<>::npos) {
            auto added_node = cur_node->GetChildPtr(bit);
            added_node->SetAttribute(rhs);
            added_node->SetFd(rhs);
            return added_node;
        }

        cur_node = cur_node->GetChild(bit);
        cur_node->SetAttribute(rhs);
    }
    cur_node->SetFd(rhs);
    return nullptr;
}

bool FDTree::ContainsFD(boost::dynamic_bitset<> const &lhs, size_t const rhs) const {
    FDTreeVertex const *cur_node = root_.get();

    for (size_t bit = lhs.find_first(); bit != boost::dynamic_bitset<>::npos;
         bit = lhs.find_next(bit)) {
        if (!cur_node->HasChildren() || !cur_node->ContainsChildAt(bit)) {
            return false;
        }

        cur_node = cur_node->GetChild(bit);
    }

    return cur_node->IsFd(rhs);
}

std::vector<boost::dynamic_bitset<>> FDTree::GetGenerals(boost::dynamic_bitset<> const &lhs,
                                                         size_t const rhs) const {
    assert(lhs.count() != 0);

    std::vector<boost::dynamic_bitset<>> result;
    boost::dynamic_bitset<> empty_lhs(GetNumAttributes());
    size_t const starting_bit = lhs.find_first();

    root_->GetGeneralsRecursive(lhs, empty_lhs, rhs, starting_bit, result);

    return result;
}

std::vector<boost::dynamic_bitset<>> FDTree::GetSpecials(boost::dynamic_bitset<> const &lhs,
                                                         size_t rhs) const {
    assert(lhs.count() != 0);

    std::vector<boost::dynamic_bitset<>> result;
    boost::dynamic_bitset<> empty_lhs(GetNumAttributes());

    root_->GetSpecialsRecursive(lhs, empty_lhs, rhs, 0, result);

    return result;
}

bool FDTree::ContainsFdOrSpecial(boost::dynamic_bitset<> const &lhs, size_t rhs) const {
    size_t next_after_last_lhs_set_bit = 0;
    if (lhs.find_first() != boost::dynamic_bitset<>::npos) {
        next_after_last_lhs_set_bit = lhs.find_first();
        while (lhs.find_next(next_after_last_lhs_set_bit) != boost::dynamic_bitset<>::npos) {
            next_after_last_lhs_set_bit = lhs.find_next(next_after_last_lhs_set_bit);
        }
        ++next_after_last_lhs_set_bit;
    }

    return root_->FindFdOrSpecialRecursive(lhs, rhs, next_after_last_lhs_set_bit, 0);
}

std::vector<LhsPair> FDTree::GetLevel(unsigned const target_level) const {
    boost::dynamic_bitset<> const empty_lhs(GetNumAttributes());

    std::vector<LhsPair> vertices;
    root_->GetLevelRecursive(target_level, 0, empty_lhs, vertices);
    return vertices;
}

std::vector<RawFD> FDTree::FillFDs() const {
    std::vector<RawFD> result;
    boost::dynamic_bitset<> lhs_for_traverse(GetRoot().GetNumAttributes());
    GetRoot().FillFDs(result, lhs_for_traverse);
    return result;
}

std::string FDTree::FDsToString() const {
    std::ostringstream oss;
    std::function<void(FDTreeVertex const *, boost::dynamic_bitset<> const &)> traverse;

    traverse = [&oss, &traverse](FDTreeVertex const *node, boost::dynamic_bitset<> const &lhs) {
        if (!node) return;
        for (size_t i = 0; i < node->GetNumAttributes(); ++i) {
            if (node->IsFd(i)) {
                oss << "FD: ";
                size_t first_bit = lhs.find_first();
                for (size_t bit = first_bit; bit != boost::dynamic_bitset<>::npos;
                     bit = lhs.find_next(bit)) {
                    if (bit != first_bit) {
                        oss << ", ";
                    }
                    oss << bit;
                }
                oss << " -> " << i << "\n";
            }
        }
        for (size_t i = 0; i < node->GetNumAttributes(); ++i) {
            if (node->ContainsChildAt(i)) {
                boost::dynamic_bitset<> new_lhs = lhs;
                new_lhs.set(i);
                traverse(node->GetChild(i), new_lhs);
            }
        }
    };

    boost::dynamic_bitset<> initial_lhs(root_->GetNumAttributes());
    traverse(root_.get(), initial_lhs);
    return oss.str();
}
}  // namespace model::dynfd
