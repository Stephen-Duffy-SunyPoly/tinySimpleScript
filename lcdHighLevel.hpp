#pragma once
#include "common.hpp"

class UpdateFunction: HighLevelConstruct {
    public:
    std::vector<PartialInstruction> expand() override;
};