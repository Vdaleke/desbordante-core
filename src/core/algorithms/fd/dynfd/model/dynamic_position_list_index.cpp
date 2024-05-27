#include "dynamic_position_list_index.h"

#include <easylogging++.h>

namespace model::dynfd {

DynamicPositionListIndex::DynamicPositionListIndex(std::list<Cluster> clusters,
                                                   std::unordered_map<int, Cluster*> inverted_index,
                                                   std::unordered_map<int, int> hash_index,
                                                   unsigned int size)
    : clusters_(std::move(clusters)),
      inverted_index_(std::move(inverted_index)),
      hash_index_(std::move(hash_index)),
      size_(size) {}

std::unique_ptr<DynamicPositionListIndex> DynamicPositionListIndex::CreateFor(
        std::vector<int> const& data) {
    std::list<Cluster> clusters;
    std::unordered_map<int, Cluster*> inverted_index;
    std::unordered_map<int, int> hash_index;
    unsigned int size = data.size();

    int id = 0;
    for (auto value_id : data) {
        hash_index[id] = value_id;
        if (inverted_index.find(value_id) == inverted_index.end()) {
            clusters.emplace_back();
            inverted_index[value_id] = &clusters.back();
        }
        (*inverted_index[value_id]).insert(static_cast<int>(id++));
    }

    return std::make_unique<DynamicPositionListIndex>(
            std::move(clusters), std::move(inverted_index), std::move(hash_index), size);
}

void DynamicPositionListIndex::Erase(int record_id) {
    int value_id = hash_index_[record_id];
    (*inverted_index_[value_id]).erase(record_id);
    hash_index_.erase(record_id);
    size_--;
}

void DynamicPositionListIndex::Insert(int record_id, int value_id) {
    hash_index_[record_id] = value_id;
    if (inverted_index_.find(value_id) == inverted_index_.end()) {
        clusters_.emplace_back();
        inverted_index_[value_id] = &clusters_.back();
    }
    (*inverted_index_[value_id]).insert(record_id);
    size_++;
}

unsigned int DynamicPositionListIndex::GetSize() const {
    return size_;
}

std::unique_ptr<DynamicPositionListIndex> DynamicPositionListIndex::FullIntersect(
        DynamicPositionListIndex const* that) const {
    std::unordered_map<int, Cluster> partial_index;
    std::list<Cluster> new_clusters;
    std::unordered_map<int, Cluster*> new_inverted_index;
    std::unordered_map<int, int> new_hash_index;
    unsigned int new_size = 0;

    for (auto& [record_id, value_id] : hash_index_) {
        if (that->hash_index_.find(record_id) == that->hash_index_.end()) {
            LOG(WARNING) << "Record id " << record_id << " not found in that index";
            continue;
        }
        int that_value_id = that->hash_index_.at(record_id);
        if (partial_index.find(that_value_id) == partial_index.end()) {
            partial_index[that_value_id] = Cluster();
        }
        partial_index[that_value_id].insert(record_id);
    }

    for (auto& [value_id, cluster] : partial_index) {
        new_clusters.push_back(cluster);
        new_inverted_index[value_id] = &new_clusters.back();
        new_size += cluster.size();
    }

    return std::make_unique<DynamicPositionListIndex>(std::move(new_clusters),
                                                      std::move(new_inverted_index),
                                                      std::move(new_hash_index), new_size);
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

}  // namespace model::dynfd
