#pragma once

#include <list>
#include <memory>
#include <set>
#include <table/position_list_index.h>
#include <unordered_map>
#include <vector>

namespace algos::dynfd {
class DynamicPositionListIndex {
public:
    class Cluster {
        std::list<size_t> records_;  // contains record ids in sorted order
        std::unordered_map<size_t, std::list<size_t>::iterator>
                position_by_record_id_;  // record_id -> iterator

    public:
        Cluster() = default;
        explicit Cluster(std::vector<size_t> unsorted_records);

        void PushBack(size_t record_id);

        void Erase(size_t record_id);

        size_t Back() const;

        bool Empty() const;

        // Iterator support
        // NOLINTBEGIN(*-identifier-naming)
        auto begin() {
            return records_.begin();
        }

        auto end() {
            return records_.end();
        }

        auto begin() const {
            return records_.begin();
        }

        auto end() const {
            return records_.end();
        }

        // NOLINTEND(*-identifier-naming)
    };

private:
    std::list<Cluster> clusters_;
    std::unordered_map<int, std::list<Cluster>::iterator>
            inverted_index_;                      // value -> cluster iterator
    std::unordered_map<size_t, int> hash_index_;  // record_id -> value
    int next_record_id_;
    unsigned int size_;
    model::ColumnIndex columnIndex_;

public:
    DynamicPositionListIndex(std::list<Cluster> clusters,
                             std::unordered_map<int, std::list<Cluster>::iterator> inverted_index,
                             std::unordered_map<size_t, int> hash_index, int next_record_id,
                             unsigned int size, model::ColumnIndex columnIndex);

    static std::unique_ptr<DynamicPositionListIndex> CreateFor(std::vector<int> const& data,
                                                               model::ColumnIndex columnIndex);

    unsigned int GetSize() const;

    Cluster const& GetCluster(int cluster_id);

    unsigned int GetClustersNum() const;

    model::ColumnIndex GetColumnIndex() const;

    int GetRecordValue(size_t record_id) const;

    size_t Insert(int value_id);

    void Erase(size_t record_id);

    std::unique_ptr<DynamicPositionListIndex> FullIntersect(
            DynamicPositionListIndex const& that) const;

    std::string ToString() const;

    std::unordered_map<size_t, int> const& GetHashIndex() const;

    // Iterator support
    // NOLINTBEGIN(*-identifier-naming)
    auto begin() {
        return clusters_.begin();
    }

    auto end() {
        return clusters_.end();
    }

    auto begin() const {
        return clusters_.begin();
    }

    auto end() const {
        return clusters_.end();
    }

    // NOLINTEND(*-identifier-naming)
};

using DPLI = DynamicPositionListIndex;
}  // namespace algos::dynfd