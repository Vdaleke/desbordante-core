#include "compressed_column_data.h"

namespace model {

CompressedColumnData::CompressedColumnData(
        Column const* column, std::unique_ptr<DynamicPositionListIndex> position_list_index)
    : AbstractColumnData(column), position_list_index_(std::move(position_list_index)) {}

std::string CompressedColumnData::ToString() const {
    return "Data for " + column_->ToString();
}

size_t CompressedColumnData::GetNumRows() const {
    return position_list_index_->GetSize();
}

}  // namespace model
