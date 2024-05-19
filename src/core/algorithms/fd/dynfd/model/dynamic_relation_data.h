#pragma once

#include <memory>

#include "model/table/idataset_stream.h"

namespace model {

class DynamicRelationData {
    static std::unique_ptr<DynamicRelationData> CreateFrom(model::IDatasetStream& data_stream);
};

}  // namespace model
