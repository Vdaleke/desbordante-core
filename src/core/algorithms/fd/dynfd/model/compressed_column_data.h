#pragma once
#include <memory>
#include <vector>

#include "dynamic_position_list_index.h"
#include "model/table/abstract_column_data.h"

namespace model {

class CompressedColumnData : AbstractColumnData {
    std::shared_ptr<DynamicPositionListIndex> position_list_index_;
    vector<int> &column_data_;
};

}  // namespace model
