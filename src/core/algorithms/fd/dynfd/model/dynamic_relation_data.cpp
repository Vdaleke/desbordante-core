#include "dynamic_relation_data.h"

namespace model {

std::unique_ptr<DynamicRelationData> DynamicRelationData::CreateFrom(
        model::IDatasetStream& data_stream) {
    auto schema = std::make_unique<RelationalSchema>(data_stream.GetRelationName());
    std::unordered_map<std::string, int> value_dictionary;
    int next_value_id = 1;
    int const null_value_id = kNullValueId;
    size_t const num_columns = data_stream.GetNumberOfColumns();
    std::vector<std::vector<int>> column_vectors = std::vector<std::vector<int>>(num_columns);
    std::vector<std::string> row;

    while (data_stream.HasNextRow()) {
        row = data_stream.GetNextRow();

        if (row.size() != num_columns) {
            LOG(WARNING) << "Unexpected number of columns for a row, skipping (expected "
                         << num_columns << ", got " << row.size() << ")";
            continue;
        }

        for (size_t index = 0; index < row.size(); ++index) {
            std::string const& field = row[index];
            if (field.empty()) {
                column_vectors[index].push_back(null_value_id);
            } else {
                auto location = value_dictionary.find(field);
                int value_id;
                if (location == value_dictionary.end()) {
                    value_dictionary[field] = next_value_id;
                    value_id = next_value_id;
                    next_value_id++;
                } else {
                    value_id = location->second;
                }
                column_vectors[index].push_back(value_id);
            }
        }
    }

    std::vector<ColumnData> column_data;
    for (size_t i = 0; i < num_columns; ++i) {
        auto column = Column(schema.get(), data_stream.GetColumnName(i), i);
        schema->AppendColumn(std::move(column));
        auto pli = model::PositionListIndex::CreateFor(column_vectors[i], is_null_eq_null);
        column_data.emplace_back(schema->GetColumn(i), std::move(pli));
    }

    schema->Init();

    return std::make_unique<ColumnLayoutRelationData>(std::move(schema), std::move(column_data));
}

}  // namespace model
