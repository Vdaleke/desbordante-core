#pragma once

#include <FDTrees/fd_tree.h>

namespace dynfd {

/**
 * Node of NonFD prefix tree.
 *
 * LHS of the FD is represented by the path to the node, besides the path must be built in ascending
 * order, i.e. LHS {0, 1} can be obtained by getting child with position 0, then its child with
 * position 1. If we go first to child 1, it will not contain child 0.
 *
 * RHS of the FD is represented by the fds attribute of the node.
 */
class NonFDTreeVertex : public model::FDTreeVertex {
    template <typename VertexType>
    friend class model::FDTree;

public:
    explicit NonFDTreeVertex(size_t numAttributes) noexcept : FDTreeVertex(numAttributes) {}
};

}  // namespace dynfd
