#pragma once
#include <memory>
#include <vector>

#include "dynamic_position_list_index.h"
#include "model/table/abstract_column_data.h"

namespace model {

class CompressedColumnData : AbstractColumnData {
    std::shared_ptr<DynamicPositionListIndex> position_list_index_;

public:
    CompressedColumnData(Column const* column,
                         std::unique_ptr<DynamicPositionListIndex> position_list_index);

    [[nodiscard]] size_t GetNumRows() const;

    [[nodiscard]] std::string ToString() const final;
};

}  // namespace model
