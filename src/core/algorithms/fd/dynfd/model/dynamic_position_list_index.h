//
// Created by Matvei Smirnov based on position_list_index
// https://github.com/Vdaleke
//
#pragma once
#include <list>
#include <memory>
#include <set>
#include <unordered_map>
#include <vector>

namespace model {

class DynamicPositionListIndex {
public:
    /* Set of tuple indices */
    using Cluster = std::set<int>;

private:
    std::list<Cluster> clusters_;
    std::unordered_map<int, Cluster*> inverted_index_;  // value -> cluster
    std::unordered_map<int, int> hash_index_;           // record_id -> value
    unsigned int size_;

public:
    DynamicPositionListIndex(std::list<Cluster> clusters,
                             std::unordered_map<int, Cluster*> inverted_index, unsigned int size);
    static std::unique_ptr<DynamicPositionListIndex> CreateFor(std::vector<int> const& data);

    unsigned int GetSize() const {
        return size_;
    }

    // std::unique_ptr<DynamicPositionListIndex> Intersect(DynamicPositionListIndex const* that)
    // const;

    std::string ToString() const;
};

using DPLI = DynamicPositionListIndex;

}  // namespace model
