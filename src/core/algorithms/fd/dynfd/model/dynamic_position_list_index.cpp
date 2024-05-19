#include "dynamic_position_list_index.h"

namespace model {

std::unique_ptr<PositionListIndex> CreateFor(std::vector<int>& data) {
    for (unsigned long position = 0; position < data.size(); ++position) {
        int value_id = data[position];
        inverted_index_[value_id].insert(position);
    }

    for (auto const& [value_id, positions] : inverted_index_) {
        clusters_.push_back(positions);
    }
}

}  // namespace model
