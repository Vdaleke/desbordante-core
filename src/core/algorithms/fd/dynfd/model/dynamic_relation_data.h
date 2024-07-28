#pragma once

#include <memory>

#include "compressed_column_data.h"
#include "model/table/idataset_stream.h"
#include "model/table/relation_data.h"

namespace dynfd {

class DynamicRelationData : public AbstractRelationData<CompressedColumnData> {
    [[nodiscard]] size_t GetNumRows() const final {
        if (column_data_.empty()) {
            return 0;
        }
        return column_data_[0].GetNumRows();
    }

public:
    explicit DynamicRelationData(std::unique_ptr<RelationalSchema> schema,
                                 std::vector<ColumnType> column_data)
        : AbstractRelationData(std::move(schema), std::move(column_data)) {}

    static std::unique_ptr<DynamicRelationData> CreateFrom(model::IDatasetStream& data_stream);
};

}  // namespace dynfd
