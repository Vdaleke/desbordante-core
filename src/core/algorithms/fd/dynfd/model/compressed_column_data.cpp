//
// Created by Матвей Смирнов on 12.05.2024.
//

#include "compressed_column_data.h"

namespace model {

CompressedColumnData::CompressedColumnData(
        Column const* column, std::unique_ptr<DynamicPositionListIndex> position_list_index,
        std::vector<int> column_data)
    : AbstractColumnData(column),
      position_list_index_(std::move(position_list_index)),
      column_data_(std::move(column_data)) {}

}  // namespace model