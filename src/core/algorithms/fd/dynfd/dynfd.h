#pragma once
#include <FDTrees/fd_tree.h>
#include <algorithm.h>
#include <tabular_data/input_table_type.h>

#include "model/dynamic_relation_data.h"
#include "model/non_fd_tree_vertex.h"

namespace dynfd {
class DynFD final : algos::Algorithm {
    config::InputTable input_table_;
    config::InputTable insert_statements_table_ = nullptr;
    config::InputTable update_statements_table_ = nullptr;
    std::unordered_set<size_t> delete_statement_indices_;
    std::shared_ptr<DynamicRelationData> relation_ = nullptr;
    std::shared_ptr<model::FDTree<>> positive_cover_tree_ = nullptr;
    std::shared_ptr<model::FDTree<NonFDTreeVertex>> negative_cover_tree_ = nullptr;

public:
    [[nodiscard]] DynamicRelationData const& GetRelation() const;

private:
    void ResetState() override;
    void LoadDataInternal() override;
    unsigned long long ExecuteInternal() override;
};

}  // namespace dynfd
