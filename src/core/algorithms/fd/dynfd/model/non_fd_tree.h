#pragma once

#include <memory>
#include <vector>

#include <boost/dynamic_bitset.hpp>

#include "algorithms/fd/raw_fd.h"
#include "non_fd_tree_vertex.h"

namespace algos::dynfd {

/**
 * NonFD prefix tree.
 *
 * Provides global tree manipulation and traversing methods.
 *
 * @see NonFDTreeVertex
 */

class NonFDTree {
private:
    std::shared_ptr<NonFDTreeVertex> root_;

public:
    explicit NonFDTree(size_t num_attributes)
        : root_(std::make_shared<NonFDTreeVertex>(num_attributes)) {}

    [[nodiscard]] size_t GetNumAttributes() const noexcept {
        return root_->GetNumAttributes();
    }

    std::shared_ptr<NonFDTreeVertex> GetRootPtr() noexcept {
        return root_;
    }

    [[nodiscard]] NonFDTreeVertex const& GetRoot() const noexcept {
        return *root_;
    }

    std::shared_ptr<NonFDTreeVertex> AddNonFD(boost::dynamic_bitset<> const& lhs, size_t rhs,
                                              ViolatingRecordPair violationPair);

    bool ContainsNonFD(boost::dynamic_bitset<>& lhs, size_t rhs);

    std::shared_ptr<NonFDTreeVertex> FindNonFdVertex(boost::dynamic_bitset<> const& lhs);

    /**
     * Recursively finds node representing given lhs and removes given rhs bit from it.
     * Destroys vertices whose children became empty.
     */
    void Remove(boost::dynamic_bitset<> const& lhs, size_t rhs) {
        root_->RemoveRecursive(lhs, rhs, lhs.find_first());
    }

    /**
     * Gets LHSs of all NonFDs having at least given lhs and rhs.

     */
    [[nodiscard]] std::vector<boost::dynamic_bitset<>> GetNonFdAndGenerals(
            boost::dynamic_bitset<>& lhs, size_t rhs) const;

    /**
     * Gets LHSs of all NonFDs having a proper subset of giving lhs and rhs.
     */
    std::vector<boost::dynamic_bitset<>> GetGenerals(boost::dynamic_bitset<>& lhs, size_t rhs);

    void RemoveGenerals(boost::dynamic_bitset<> const& lhs, size_t rhs);

    std::vector<boost::dynamic_bitset<>> GetNonFdAndSpecials(boost::dynamic_bitset<>& lhs,
                                                             size_t rhs);

    /**
     * Gets LHSs of all NonFDs having given lhs as a proper subset and rhs.
     */
    std::vector<boost::dynamic_bitset<>> GetSpecials(boost::dynamic_bitset<>& lhs, size_t rhs);

    void RemoveSpecials(boost::dynamic_bitset<>& lhs, size_t rhs);

    /**
     * Checks if any NonFD has at least given lhs and rhs.
     */
    [[nodiscard]] bool ContainsNonFdOrGeneral(boost::dynamic_bitset<> const& lhs,
                                              size_t rhs) const {
        return root_->ContainsNonFdOrGeneralRecursive(lhs, rhs, lhs.find_first());
    }

    [[nodiscard]] bool ContainsNonFdOrSpecial(boost::dynamic_bitset<>& lhs, size_t rhs) const;

    /**
     * Gets nodes representing NonFDs with LHS of given arity.
     * @param target_level arity of returned NonFDs LHSs
     */
    std::vector<LhsPair> GetLevel(unsigned target_level);

    /**
     * @return vector of all NonFDs
     */
    [[nodiscard]] std::vector<RawFD> FillNonFDs() const;
};
}  // namespace algos::dynfd