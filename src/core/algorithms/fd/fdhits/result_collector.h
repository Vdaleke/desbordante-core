#pragma once

#include <chrono>
#include <unordered_map>
#include <vector>

#include "model/table/column.h"
#include "model/table/column_combination.h"

namespace algos::fdhits {

namespace timer {
enum class TimerName {
    TOTAL,
    CONSTRUCT_CLUSTERS,
    HITTING_SET,
    VALIDATE,
    MISC
};
}  // namespace timer

class ResultCollector {
private:
    double timeout_;
    std::chrono::steady_clock::time_point start_total_;

    std::unordered_map<timer::TimerName, std::chrono::duration<double>> durations_;
    std::unordered_map<timer::TimerName, std::chrono::steady_clock::time_point> current_starts_;

    // Обнаруженные функциональные зависимости (LHS -> RHS)
    std::vector<std::pair<model::ColumnCombination, model::ColumnIndex>> fds_;

    // Количество проверенных кандидатов
    std::size_t candidates_checked_ = 0;

    // Количество найденных FD
    std::size_t fds_found_ = 0;

public:
    explicit ResultCollector(double timeout = 3600);

    void StartTimer(timer::TimerName name);
    void StopTimer(timer::TimerName name);
    double GetTimeFor(timer::TimerName name) const;

    void RegisterFD(model::ColumnCombination lhs, model::ColumnIndex rhs);
    void RegisterCandidate() { candidates_checked_++; }

    std::size_t GetCandidatesChecked() const { return candidates_checked_; }
    std::size_t GetFDsFound() const { return fds_found_; }

    std::vector<std::pair<model::ColumnCombination, model::ColumnIndex>> const& GetFDs() const { return fds_; }

    void CheckTimeout() const;
};

}  // namespace algos::fdhits
