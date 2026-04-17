#pragma once
#include <memory>
#include "common.hpp"


class VariableAssignment: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
    std::unique_ptr<DataType> value;
public:
    explicit VariableAssignment([[maybe_unused]] const std::string& name, const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};
