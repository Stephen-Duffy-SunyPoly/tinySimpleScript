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

class IncrementHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
public:
    explicit IncrementHighLevelOperation([[maybe_unused]] const std::string& name);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class DecrementHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> var;
public:
    explicit DecrementHighLevelOperation([[maybe_unused]] const std::string& name);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class UserFunctionHighLevelOperation: public HighLevelConstruct {
    std::vector<std::unique_ptr<HighLevelConstruct>> blocks;
    std::string name;
    //TODO args
public:
    explicit UserFunctionHighLevelOperation(std::string  name, const std::string &params,std::ifstream& file, int & lineNumber);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class CallUserFunctionHighLevelOperation: public HighLevelConstruct {
    std::string name;
    std::vector<std::unique_ptr<DataType>> params;//TODO
public:
    explicit CallUserFunctionHighLevelOperation([[maybe_unused]] const std::string& name): name(name) {}
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class TrapHighLevelOperation: public HighLevelConstruct {
public:
    explicit TrapHighLevelOperation([[maybe_unused]] const std::string& line){}
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};
