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
    std::vector<std::string> localVars;
    std::vector<std::string> paramaters;
    bool returnsValue;
    std::unique_ptr<DataType> returnValue;
public:
    explicit UserFunctionHighLevelOperation(std::string  &name, const std::string &params,std::ifstream& file, int & lineNumber, std::vector<UserFunctionData>& functionData);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class CallUserFunctionHighLevelOperation: public HighLevelConstruct {
    std::string name;
    std::vector<std::unique_ptr<DataType>> params;
    std::unique_ptr<DataType> returnValueTo;
public:
    explicit CallUserFunctionHighLevelOperation(std::string  name, const std::string &rawParams, const std::string &returnTo);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class TrapHighLevelOperation: public HighLevelConstruct {
public:
    explicit TrapHighLevelOperation([[maybe_unused]] const std::string& line){}
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class ReturnHighLevelOperation: public HighLevelConstruct {
public:
    std::unique_ptr<DataType> value;
    explicit ReturnHighLevelOperation(const std::string &value);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class LoopHighLevelOperation: public HighLevelConstruct {
    std::vector<std::unique_ptr<HighLevelConstruct>> blocks;
    std::string name;
    std::vector<std::string> localVars;
    std::unique_ptr<DataType> check1;
    std::unique_ptr<DataType> check2;
    ConditionType condition;
public:
    explicit LoopHighLevelOperation(std::ifstream& file, int & lineNumber);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class IfHighLevelOperation: public HighLevelConstruct {
    std::vector<std::unique_ptr<HighLevelConstruct>> trueBlocks;
    std::vector<std::unique_ptr<HighLevelConstruct>> falseBlocks;
    std::string name;
    std::vector<std::string> trueLocalVars;
    std::vector<std::string> falseLocalVars;
    std::unique_ptr<DataType> check1;
    std::unique_ptr<DataType> check2;
    ConditionType condition;
public:
    explicit IfHighLevelOperation(const std::string& conditionRaw, std::ifstream& file, int & lineNumber);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class DelayFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> amount;
public:
    explicit DelayFunction([[maybe_unused]] const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class RandFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnTo;
public:
    explicit RandFunction(const std::string &retVar, const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};
class RandBitsFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnTo;
public:
    explicit RandBitsFunction(const std::string &retVar, const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class ReadPortAFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnTo;
public:
    explicit ReadPortAFunction(const std::string &retVar, const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class ReadPortBFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnTo;
public:
    explicit ReadPortBFunction(const std::string &retVar, const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class SetPortAFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> setTo;
public:
    explicit SetPortAFunction(const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class SetPortBFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> setTo;
public:
    explicit SetPortBFunction(const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class SetPortADirectionFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> setTo;
public:
    explicit SetPortADirectionFunction(const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class SetPortBDirectionFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> setTo;
public:
    explicit SetPortBDirectionFunction(const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class RightShiftHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> data;
    std::unique_ptr<DataType> amount;
public:
    RightShiftHighLevelOperation(const std::string& var, const std::string &by);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class LeftShiftHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> data;
    std::unique_ptr<DataType> amount;
public:
    LeftShiftHighLevelOperation(const std::string& var, const std::string &by);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class NegateHighLevelOperation: public HighLevelConstruct {
    std::unique_ptr<DataType> data;
public:
    NegateHighLevelOperation(const std::string& var);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};