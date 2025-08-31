#pragma once

#include <vector>

#include <boost/dynamic_bitset.hpp>

namespace algos::fdhits {

typedef boost::dynamic_bitset<> Edge;

class Hypergraph {
private:
    Edge::size_type num_vertices_;
    std::vector<Edge> edges_;

public:
    Hypergraph() = delete;
    explicit Hypergraph(Edge::size_type num_vertices);

    std::vector<Edge>::size_type NumEdges() const {
        return edges_.size();
    }

    Edge::size_type NumVertices() const {
        return num_vertices_;
    }

    void AddEdge(Edge const& new_edge) {
        edges_.push_back(new_edge);
    }

    void AddEdge(Edge&& new_edge) {
        edges_.push_back(new_edge);
    }

    void RemoveLastEdge() {
        edges_.pop_back();
    }

    void AddEdgeAndMinimizeInclusion(Edge const& new_edge);

    // операторы и связанные методы

    Edge& operator[](std::vector<Edge>::size_type i_e) {
        return edges_[i_e];
    }

    Edge const& operator[](std::vector<Edge>::size_type i_e) const {
        return edges_[i_e];
    }
};

}  // namespace algos::fdhits
