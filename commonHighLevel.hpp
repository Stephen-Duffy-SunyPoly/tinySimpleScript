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


class AddHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
    std::unique_ptr<DataType> value;
public:
    explicit AddHighLevelOperation([[maybe_unused]] const std::string& name, const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class SubtractHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
    std::unique_ptr<DataType> value;
public:
    explicit SubtractHighLevelOperation([[maybe_unused]] const std::string& name, const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class MultiplyHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
    std::unique_ptr<DataType> value;
public:
    explicit MultiplyHighLevelOperation([[maybe_unused]] const std::string& name, const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class DivideHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
    std::unique_ptr<DataType> value;
public:
    explicit DivideHighLevelOperation([[maybe_unused]] const std::string& name, const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class ModulusHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
    std::unique_ptr<DataType> value;
public:
    explicit ModulusHighLevelOperation([[maybe_unused]] const std::string& name, const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class AndHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
    std::unique_ptr<DataType> value;
public:
    explicit AndHighLevelOperation([[maybe_unused]] const std::string& name, const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class OrHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
    std::unique_ptr<DataType> value;
public:
    explicit OrHighLevelOperation([[maybe_unused]] const std::string& name, const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class XorHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
    std::unique_ptr<DataType> value;
public:
    explicit XorHighLevelOperation([[maybe_unused]] const std::string& name, const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};