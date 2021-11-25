#pragma once

#include <random>
#include <stack>

#include "FDAlgorithm.h"
#include "Vertical.h"
#include "DFD/PartitionStorage/PartitionStorage.h"

class DFD : public FDAlgorithm {
private:
    std::unique_ptr<PartitionStorage> partitionStorage;
    std::vector<Vertical> uniqueColumns;

    unsigned int numberOfThreads;

    unsigned long long executeInternal() override;
public:
    explicit DFD(std::filesystem::path const& path,
                 char separator = ',', bool hasHeader = true,
                 unsigned int parallelism = 0);
};