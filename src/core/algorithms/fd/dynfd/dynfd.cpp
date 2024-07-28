#include "dynfd.h"

#include "algo_factory.h"
#include "fd/hycommon/all_column_combinations.h"
#include "fd/hyfd/hyfd.h"

namespace dynfd {

DynamicRelationData const& DynFD::GetRelation() const {
    assert(relation_ != nullptr);
    return *relation_;
}

void DynFD::ResetState() {}

void DynFD::LoadDataInternal() {
    relation_ = DynamicRelationData::CreateFrom(*input_table_);

    if (relation_->GetColumnData().empty()) {
        throw std::runtime_error("Got an empty dataset: FD mining is meaningless.");
    }

    std::shared_ptr hy_fd_relation = ColumnLayoutRelationData::CreateFrom(*input_table_, false);

    algos::hyfd::HyFD hyfd_instance(algos::PliBasedFDAlgorithm::ColumnLayoutRelationDataManager(
            &input_table_, nullptr, &hy_fd_relation));

    hyfd_instance.LoadData();

    // Execute HyFD
    unsigned long long elapsed_milliseconds = hyfd_instance.Execute();

    positive_cover_tree_ = hyfd_instance.GetPositiveCoverTree();

    negative_cover_tree_ =
            std::make_shared<model::FDTree<NonFDTreeVertex>>(GetRelation().GetNumColumns());


}

unsigned long long DynFD::ExecuteInternal() {
    return 0;
}

}  // namespace dynfd
