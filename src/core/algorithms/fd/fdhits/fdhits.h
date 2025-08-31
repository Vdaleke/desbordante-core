
#pragma once

#include <memory>

#include "algorithms/fd/fdhits/hypergraph.h"
#include "algorithms/fd/fdhits/pli_table.h"
#include "algorithms/fd/fdhits/result_collector.h"
#include "algorithms/fd/pli_based_fd_algorithm.h"
#include "config/thread_number/type.h"

namespace algos::fdhits {

bool ValidateFD(PLITable const& tab, Edge const& lhs, model::ColumnIndex rhs);

class FdHits : public algos::PliBasedFDAlgorithm {
private:
    // Настройки
    config::ThreadNumType number_of_threads_;

    unsigned long long ExecuteInternal() override;
    PLITable Preprocess(ResultCollector& rc);
    void RegisterFDs(ResultCollector const& rc);
    void PrintInfo(ResultCollector const& rc) const;

    void RegisterOptions();
    void MakeExecuteOptsAvailableFDInternal() override;
    void ResetStateFd() override;

public:
    FdHits();

    unsigned int Fletcher16() const;
};

}  // namespace algos::fdhits
