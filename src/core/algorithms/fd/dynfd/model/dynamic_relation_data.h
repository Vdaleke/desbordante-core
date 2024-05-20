#pragma once

#include <memory>

#include "compressed_column_data.h"
#include "model/table/idataset_stream.h"
#include "model/table/relation_data.h"

namespace model {

class DynamicRelationData : AbstractRelationData<CompressedColumnData> {
    std::unordered_map<std::string, int> value_dictionary_;

    explicit DynamicRelationData(std::unique_ptr<RelationalSchema> schema,
                                 std::vector<ColumnType> column_data,
                                 std::unordered_map<std::string, int> value_dictionary)
        : AbstractRelationData(std::move(schema), std::move(column_data)),
          value_dictionary_(std::move(value_dictionary)) {}

    static std::unique_ptr<DynamicRelationData> CreateFrom(model::IDatasetStream& data_stream);
};

}  // namespace model
