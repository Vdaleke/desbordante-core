#include "dynamic_relation_data.h"

#include <unordered_map>

#include <easylogging++.h>

namespace model {

std::unique_ptr<DynamicRelationData> DynamicRelationData::CreateFrom(
        model::IDatasetStream& data_stream) {
    auto schema = std::make_unique<RelationalSchema>(data_stream.GetRelationName());
    int next_value_id = 1;
    std::unordered_map<std::string, int> value_dictionary;
    size_t const num_columns = data_stream.GetNumberOfColumns();
    std::vector<std::vector<int>> column_dictionary_encoded_data =
            std::vector<std::vector<int>>(num_columns);
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

            auto location = value_dictionary.find(field);
            int value_id;
            if (location == value_dictionary.end()) {
                value_id = next_value_id;
                value_dictionary[field] = value_id;
                next_value_id++;
            } else {
                value_id = location->second;
            }

            column_dictionary_encoded_data[index].push_back(value_id);
        }
    }

    std::vector<CompressedColumnData> column_data;
    for (size_t i = 0; i < num_columns; ++i) {
        auto column = Column(schema.get(), data_stream.GetColumnName(i), i);
        schema->AppendColumn(std::move(column));
        auto pli = DynamicPositionListIndex::CreateFor(column_dictionary_encoded_data[i]);
        column_data.emplace_back(schema->GetColumn(i), std::move(pli),
                                                    std::move(column_dictionary_encoded_data[i]));
    }

    schema->Init();

    return std::make_unique<DynamicRelationData>(std::move(schema), std::move(column_data),
                                                 std::move(value_dictionary));
}

}  // namespace model
