#include "result_collector.h"

#include <stdexcept>

namespace algos::fdhits {

ResultCollector::ResultCollector(double timeout) : timeout_(timeout) {
    start_total_ = std::chrono::steady_clock::now();
    StartTimer(timer::TimerName::TOTAL);
}

void ResultCollector::StartTimer(timer::TimerName name) {
    current_starts_[name] = std::chrono::steady_clock::now();
}

void ResultCollector::StopTimer(timer::TimerName name) {
    std::chrono::steady_clock::time_point stop = std::chrono::steady_clock::now();
    
    if (current_starts_.find(name) == current_starts_.end()) {
        return;
    }
    
    std::chrono::steady_clock::time_point start = current_starts_[name];
    if (durations_.find(name) == durations_.end()) {
        durations_[name] = std::chrono::duration<double>(0);
    }
    
    durations_[name] += std::chrono::duration<double>(stop - start);
}

double ResultCollector::GetTimeFor(timer::TimerName name) const {
    if (durations_.find(name) == durations_.end()) {
        return 0.0;
    }
    
    return durations_.at(name).count();
}

void ResultCollector::RegisterFD(model::ColumnCombination lhs, model::ColumnIndex rhs) {
    fds_.emplace_back(std::move(lhs), rhs);
    fds_found_++;
}

void ResultCollector::CheckTimeout() const {
    auto now = std::chrono::steady_clock::now();
    std::chrono::duration<double> elapsed = now - start_total_;
    if (elapsed.count() > timeout_) {
        throw std::runtime_error("FD-HITS execution timed out");
    }
}

}  // namespace algos::fdhits
