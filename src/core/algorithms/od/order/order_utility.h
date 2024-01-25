#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/container_hash/hash.hpp>

#include "model/table/column_index.h"

namespace algos::order {
using Node = std::vector<model::ColumnIndex>;
using AttributeList = std::vector<model::ColumnIndex>;
using CandidatePairs = std::vector<std::pair<AttributeList, AttributeList>>;
using ListHash = boost::hash<AttributeList>;
using CandidateSets =
        std::unordered_map<AttributeList, std::unordered_set<AttributeList, ListHash>, ListHash>;
using OrderDependencies =
        std::unordered_map<AttributeList, std::unordered_set<AttributeList, ListHash>, ListHash>;

void PrintOD(AttributeList const& lhs, AttributeList const& rhs);
std::vector<AttributeList> GetPrefixes(Node const& node);
AttributeList MaxPrefix(AttributeList const& attribute_list);
bool InUnorderedMap(OrderDependencies const& map, AttributeList const& lhs,
                    AttributeList const& rhs);
bool AreDisjoint(AttributeList const& a, AttributeList const& b);
bool StartsWith(AttributeList const& rhs_candidate, AttributeList const& rhs);
}  // namespace algos::order