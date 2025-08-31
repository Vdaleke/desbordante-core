#pragma once

#include <deque>
#include <random>
#include <stack>
#include <vector>

#include "algorithms/fd/fdhits/edgemark.h"
#include "algorithms/fd/fdhits/hypergraph.h"
#include "model/table/position_list_index.h"

namespace algos::fdhits {

struct Config;
struct PLITable;
class ResultCollector;

class TreeSearch {
private:
    PLITable const& tab_;
    Config const& cfg_;
    ResultCollector& rc_;

    // частичный гиперграф множеств различий
    Hypergraph partial_hg_;

    // исключение для выброса при тайм-ауте
    unsigned const timeout_ = 10;

    // отображение из clusterid в индексы записей, используется для пересечения
    // PLI с одиночными PLI столбцов
    std::vector<::model::PLI::Cluster> clusterid_to_recordindices_;

    // отображение от столбца к его "красивости" (в [0, nr_cols)),
    // меньшие значения соответствуют более "красивым" колонкам
    std::vector<unsigned long> niceness_;
    void ComputeNiceness();
    unsigned long Niceness(Edge const& e) const;

    std::default_random_engine gen_;
    Hypergraph Sample(std::deque<::model::PLI::Cluster> const& pli);

    inline void UpdateCritAndUncov(std::vector<std::vector<Edgemark>>& removed_criticals_stack,
                                   std::vector<Edgemark>& crit, Edgemark& uncov,
                                   Edgemark const& v_hittings) const;
    inline void RestoreCritAndUncov(std::vector<std::vector<Edgemark>>& removed_criticals_stack,
                                    std::vector<Edgemark>& crit, Edgemark& uncov) const;

    inline bool ExtendOrConfirmS(Edge& s, Edge& cand, std::vector<Edgemark>& crit, Edgemark& uncov,
                                 std::vector<std::vector<Edgemark>>& removed_criticals_stack,
                                 std::vector<Edge>& minimal_transversals);

public:
    TreeSearch(PLITable const& tab, Config const& cfg, ResultCollector& rc);

    std::vector<Edge> ComputeMinimalHittingSets();
    void AddAllPossibleEdges();
};

}  // namespace algos::fdhits
