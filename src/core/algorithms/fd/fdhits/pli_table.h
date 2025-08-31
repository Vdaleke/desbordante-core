#pragma once

#include <deque>
#include <limits>
#include <vector>

#include "algorithms/fd/hycommon/types.h"
#include "model/table/position_list_index.h"
#include "model/table/tuple_index.h"

namespace algos::fdhits {

// Таблица в форме PLI вместе с обратным отображением и
// дополнительной информацией
struct PLITable {
    // PLI: для каждой колонки у нас есть вектор кластеров,
    // где каждый кластер - это вектор идентификаторов строк
    std::vector<std::deque<model::PositionListIndex::Cluster>> plis;

    // Обратное отображение: для каждой колонки у нас есть вектор, отображающий
    // идентификатор строки в идентификатор кластера
    algos::hy::Columns inverse_mapping;

    // Количество строк
    model::TupleIndex nr_rows;

    // Количество колонок
    model::ColumnIndex nr_cols;
};

// ID кластера для всех кластеров размера 1
constexpr unsigned kSizeOneCluster = std::numeric_limits<unsigned>::max();

}  // namespace algos::fdhits
