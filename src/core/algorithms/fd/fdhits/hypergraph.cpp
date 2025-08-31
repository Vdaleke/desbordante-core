#include "hypergraph.h"

namespace algos::fdhits {

Hypergraph::Hypergraph(Edge::size_type num_vertices) : num_vertices_(num_vertices) {}

void Hypergraph::AddEdgeAndMinimizeInclusion(Edge const& new_edge) {
    // Удаляем все существующие ребра, которые содержат новое ребро как подмножество
    auto iter = edges_.begin();
    while (iter != edges_.end()) {
        // Проверяем, является ли new_edge подмножеством iter
        // Если (a & b) == a, то a является подмножеством b
        Edge intersection = new_edge;
        Edge current = *iter;
        
        bool is_subset = true;
        for (size_t i = 0; i < new_edge.size(); i++) {
            if (new_edge[i] && !current[i]) {
                is_subset = false;
                break;
            }
        }
        
        if (is_subset) {
            iter = edges_.erase(iter);
        } else {
            ++iter;
        }
    }
    
    // Проверяем, является ли новое ребро надмножеством любого существующего ребра
    for (auto const& edge : edges_) {
        bool is_subset = true;
        for (size_t i = 0; i < edge.size(); i++) {
            if (edge[i] && !new_edge[i]) {
                is_subset = false;
                break;
            }
        }
        
        if (is_subset) {
            return; // Существующее ребро является подмножеством нового, поэтому новое не минимально
        }
    }
    
    // Добавляем новое ребро, так как оно минимально
    edges_.push_back(new_edge);
}

}  // namespace algos::fdhits
