#include "dynamic_position_list_index.h"

namespace model {

DynamicPositionListIndex::DynamicPositionListIndex(std::list<Cluster> clusters, std::unordered_map<int, std::set<int>> inverted_index, unsigned int size)
        : clusters_(std::move(clusters)), inverted_index_(std::move(inverted_index)), size_(size) {
    }

std::unique_ptr<DynamicPositionListIndex> DynamicPositionListIndex::CreateFor(std::vector<int>& data) {
    std::list<Cluster> clusters;
    std::unordered_map<int, std::set<int>> inverted_index;
    unsigned int size;

    size = data.size();
    for (unsigned long position = 0; position < size; ++position) {
        int value_id = data[position];
        inverted_index[value_id].insert(static_cast<int>(position));
    }

    for (auto const& [value_id, positions] : inverted_index) {
        clusters.push_back(positions);
    }

    return std::make_unique<DynamicPositionListIndex>(std::move(clusters), std::move(inverted_index), size);
}

}  // namespace model
