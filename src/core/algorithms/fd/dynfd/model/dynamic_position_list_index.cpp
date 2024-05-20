#include "dynamic_position_list_index.h"

namespace model {

DynamicPositionListIndex::DynamicPositionListIndex(std::list<Cluster> clusters,
                                                   std::unordered_map<int, Cluster*> inverted_index,
                                                   unsigned int size)
    : clusters_(std::move(clusters)), inverted_index_(std::move(inverted_index)), size_(size) {}

std::unique_ptr<DynamicPositionListIndex> DynamicPositionListIndex::CreateFor(
        std::vector<int>& data) {
    std::list<Cluster> clusters;
    std::unordered_map<int, Cluster*> inverted_index;
    unsigned int size;

    size = data.size();
    for (unsigned long position = 0; position < size; ++position) {
        int value_id = data[position];
        if (inverted_index.find(value_id) == inverted_index.end()) {
            clusters.emplace_back();
            inverted_index[value_id] = &clusters.back();
        }
        (*inverted_index[value_id]).insert(static_cast<int>(position));
    }

    return std::make_unique<DynamicPositionListIndex>(std::move(clusters),
                                                      std::move(inverted_index), size);
}

std::string DynamicPositionListIndex::ToString() const {
    std::string res = "[";
    for (auto& cluster : clusters_) {
        res.push_back('[');
        for (int v : cluster) {
            res.append(std::to_string(v) + ", ");
        }
        res.erase(res.size() - 2);
        res.push_back(']');
        res += ", ";
    }
    res.erase(res.size() - 2);
    res.push_back(']');
    return res;
}

}  // namespace model
