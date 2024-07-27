#pragma once

#include <memory>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "algorithms/fd/raw_fd.h"
#include "fd_tree_vertex.h"

namespace model::dynfd {
/**
 * FD prefix tree.
 *
 * Provides global tree manipulation and traversing methods.
 *
 * @see FDTreeVertex
 */
class FDTree {
private:
    std::shared_ptr<FDTreeVertex> root_;

public:
    explicit FDTree(size_t num_attributes) : root_(std::make_shared<FDTreeVertex>(num_attributes)) {
        for (size_t id = 0; id < num_attributes; id++) {
            root_->SetFd(id);
        }
    }

    [[nodiscard]] size_t GetNumAttributes() const noexcept {
        return root_->GetNumAttributes();
    }

    std::shared_ptr<FDTreeVertex> GetRootPtr() noexcept {
        return root_;
    }

    [[nodiscard]] FDTreeVertex const &GetRoot() const noexcept {
        return *root_;
    }

    std::shared_ptr<FDTreeVertex> AddFD(boost::dynamic_bitset<> const &lhs, size_t const rhs);

    bool ContainsFD(boost::dynamic_bitset<> const &lhs, size_t const rhs) const;

    /**
     * Recursively finds node representing given lhs and removes given rhs bit from it.
     * Destroys vertices whose children became empty.
     */
    void Remove(boost::dynamic_bitset<> const &lhs, size_t const rhs) {
        root_->RemoveRecursive(lhs, rhs, lhs.find_first());
    }

    /**
     * Gets LHSs of all FDs having a proper subset of giving lhs and rhs.
     */
    [[nodiscard]] std::vector<boost::dynamic_bitset<> > GetGenerals(
            boost::dynamic_bitset<> const &lhs, size_t rhs) const;

    /**
     * Gets LHSs of all FDs having given lhs as a proper subset and rhs.
     */
    [[nodiscard]] std::vector<boost::dynamic_bitset<> > GetSpecials(
            boost::dynamic_bitset<> const &lhs, size_t rhs) const;

    /**
     * Checks if any FD has at least given lhs and rhs.
     */
    [[nodiscard]] bool ContainsFdOrGeneral(boost::dynamic_bitset<> const &lhs, size_t rhs) const {
        return root_->FindFdOrGeneralRecursive(lhs, rhs, lhs.find_first());
    }

    /**
     * Checks if any FD has given lhs and rhs.
     */
    [[nodiscard]] bool ContainsFdOrSpecial(boost::dynamic_bitset<> const &lhs, size_t rhs) const;

    /**
     * Gets nodes representing FDs with LHS of given arity.
     * @param target_level arity of returned FDs LHSs
     */
    std::vector<LhsPair> GetLevel(unsigned target_level) const;

    /**
     * @return vector of all FDs
     */
    [[nodiscard]] std::vector<RawFD> FillFDs() const;

    /**
     * @return string representation of the tree
     */
    std::string FDsToString() const;
};
}  // namespace model::dynfd
