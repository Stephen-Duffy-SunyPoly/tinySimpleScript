#pragma once

#include <chrono>
#include <string>
#include <utility>
#include <vector>
#include <stdexcept>

const std::string WHITESPACE = " \t\n\r\f\v";

//unitiliy functions

//why does c++ not have one of these?
/**Trims a string, removing all leading and trailing whitespace
 * @param str the string to trim
 * @return A string with no leading or trailing whitespace
 */
inline std::string trim(const std::string& str) {
    const auto first = str.find_first_not_of(WHITESPACE);
    if (first == std::string::npos) return "";
    const auto last = str.find_last_not_of(WHITESPACE);
    return str.substr(first, (last - first + 1));
}

inline bool charIsNumber(char c) {
    return c >= '0' && c <= '9';
}

inline bool stringIsNumber(const std::string &str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

//structures

struct Register {
    int regNumber; //the actual register in use
    int lru = 0; // leased recently used value
    bool dirty = false;//if this register has been written too since it was loaded / last saved (if so it will have to be saved to memory before overriding
    bool immediate = false;//if the current value of this register was used for an immediate instead of a variable
    bool immediateUsed = false;//if this register currently represents and immediate and has been used. if it has not been used then do not override. if it has been used then feel free to override
    std::string varName;
    int imValue = 0;

    explicit Register(int regNumber) : regNumber(regNumber) {}
    void operator ++(int) {
        lru++;
    }
};

class DataType {
public:
    virtual ~DataType() =default;
    virtual std::string toString() = 0;
    virtual bool isVariable() {
        return false;
    }
    virtual bool needsResolve() {
        return true;
    }
    virtual std::string asAsm() = 0;
};

class VariableDataType : public DataType {
    bool stackVar = false;
    bool resolved = false;
    int offset = 0;//address if global, stack offset if local
    std::string varName;
public:
    explicit VariableDataType(std::string varName) : varName(std::move(varName)) {}
    std::string toString() override {
        return "var_"+varName;
    }
    bool isVariable() override {
        return true;
    }
    void resolve(bool stack, int addr) {
        stackVar = stack;
        offset = addr;
        resolved = true;
    }
    std::string getVarName() {
        return varName;
    }
    std::string asAsm() override;
    [[nodiscard]] int getOffset() const {
        return offset+1;
    }
};

class ImmediateDataType : public DataType {
    int value = 0;
public:
    explicit ImmediateDataType(int value) : value(value) {}
    [[nodiscard]] int getValue() const {
        return value;
    }
    std::string toString() override {
        return std::to_string(value);
    }
    std::string asAsm() override {
        return std::to_string(value);
    }
};

class ZeroDataType : public DataType {
    public:
    std::string toString() override {
        return "rz";
    }
    bool needsResolve() override {
        return false;
    }
    std::string asAsm() override {
        return "rz";
    }
};

struct UserFunctionData {
    std::string name;
    int numberOfParameters;
};

struct FinishedInstruction {
    virtual ~FinishedInstruction() = default;

    std::string operation;
    int operands;
    std::string op1;
    std::string op2;
    bool label = false;
    virtual std::string produce();
    FinishedInstruction(std::string operation,int operands,std::string op1,std::string op2): operation(std::move(operation)), operands(operands), op1(std::move(op1)), op2(std::move(op2)) {}
    FinishedInstruction(std::string operation,int operands,std::string op1,std::string op2, bool label): operation(std::move(operation)), operands(operands), op1(std::move(op1)), op2(std::move(op2)), label(label) {}
};

struct StackModificationAccountingFinishedInstruction: public FinishedInstruction {
    int stackOffset;

    StackModificationAccountingFinishedInstruction(const std::string &operation, int operands, const std::string &op1,const std::string &op2, int stack_offset)
        : FinishedInstruction(operation, operands, op1, op2),stackOffset(stack_offset) {}
    StackModificationAccountingFinishedInstruction(const std::string &operation, int operands, const std::string &op1,const std::string &op2, bool label, int stack_offset)
        : FinishedInstruction(operation, operands, op1, op2, label),stackOffset(stack_offset) {}
    std::string produce() override;
};

class RegisterResolver {
    std::vector<Register> &registers;
    std::vector<std::string> &localVars;
    std::vector<bool> registersUsed;
    std::vector<FinishedInstruction*> partiallyResolvedStackVars;
    std::vector<std::string> paramVars;
public:
    RegisterResolver(std::vector<Register> &registers, std::vector<std::string> &localVars, std::vector<std::string> paramVars) :
    registers(registers), localVars(localVars), paramVars(std::move(paramVars)) {
        registersUsed.resize(registers.size());
    }
    std::string resolve(std::unique_ptr<DataType>& data, std::vector<std::unique_ptr<FinishedInstruction>>& finishedInstructions, bool wrightOp, int existingStackOffset =0);
    //call this after your done using this resolver
    int backupRegisters(std::vector<std::unique_ptr<FinishedInstruction>>& finishedInstructions);
    void restoreRegisters(std::vector<std::unique_ptr<FinishedInstruction>>& finishedInstructions);

    //call before any function call or block code
    void flushGlobalVars(std::vector<std::unique_ptr<FinishedInstruction>>& finishedInstructions) const;
    void correctExtraStackVars(int numRegsUsed, std::vector<std::unique_ptr<FinishedInstruction>>& finishedInstructions) const;
};


class PartialInstruction {
public:
    virtual ~PartialInstruction() =default;
    virtual std::string toString() = 0;
    virtual int numVars() = 0;
    virtual std::unique_ptr<DataType>& getVariable(int vn) = 0;
    virtual std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) = 0;
    virtual void validatFunctionCalls(std::vector<UserFunctionData>& functionNames){};
    virtual std::vector<std::string> getLocalVarScope(int vn, std::vector<std::string>& outerVarNames) {
        return outerVarNames;
    }
};

//stores a value directly to a known symbol

class DirectStorPartialInstruction : public PartialInstruction {
    std::string storeTo;
    std::unique_ptr<DataType> value;
public:
    explicit DirectStorPartialInstruction(std::string storeTo, std::unique_ptr<DataType> value) : storeTo(std::move(storeTo)), value(std::move(value)) {}
    std::string toString() override {
        return "store "+value->toString()+" in "+storeTo;
    }
    int numVars() override {
        return 1;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override {
        return value;
    }
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class VariableAssignPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> to;
    std::unique_ptr<DataType> from;
public:
    VariableAssignPartialInstruction(std::unique_ptr<DataType> to, std::unique_ptr<DataType> from) : to(std::move(to)), from(std::move(from)) {}
    std::string toString() override {
        return "set " + to->toString() + " to " + from->toString();
    }
    int numVars() override {
        return 2;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class AddPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> to;
    std::unique_ptr<DataType> from;
public:
    AddPartialInstruction(std::unique_ptr<DataType> to, std::unique_ptr<DataType> from) : to(std::move(to)), from(std::move(from)) {}

    std::string toString() override {
        return "add " + from->toString() + " to " + to->toString();
    }
    int numVars() override {
        return 2;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class SubtractPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> to;
    std::unique_ptr<DataType> from;
public:
    SubtractPartialInstruction(std::unique_ptr<DataType> to, std::unique_ptr<DataType> from) : to(std::move(to)), from(std::move(from)) {}

    std::string toString() override {
        return "subtract " + from->toString() + " from " + to->toString();
    }
    int numVars() override {
        return 2;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class MultiplyPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> to;
    std::unique_ptr<DataType> from;
public:
    MultiplyPartialInstruction(std::unique_ptr<DataType> to, std::unique_ptr<DataType> from) : to(std::move(to)), from(std::move(from)) {}

    std::string toString() override {
        return "multiply " + from->toString() + " width " + to->toString();
    }
    int numVars() override {
        return 2;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class DividePartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> to;
    std::unique_ptr<DataType> from;
public:
    DividePartialInstruction(std::unique_ptr<DataType> to, std::unique_ptr<DataType> from) : to(std::move(to)), from(std::move(from)) {}

    std::string toString() override {
        return "divide " + from->toString() + " by " + to->toString();
    }
    int numVars() override {
        return 2;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class ModulusPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> to;
    std::unique_ptr<DataType> from;
public:
    ModulusPartialInstruction(std::unique_ptr<DataType> to, std::unique_ptr<DataType> from) : to(std::move(to)), from(std::move(from)) {}

    std::string toString() override {
        return "mod " + from->toString() + " by " + to->toString();
    }
    int numVars() override {
        return 2;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class AndPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> to;
    std::unique_ptr<DataType> from;
public:
    AndPartialInstruction(std::unique_ptr<DataType> to, std::unique_ptr<DataType> from) : to(std::move(to)), from(std::move(from)) {}

    std::string toString() override {
        return "and " + from->toString() + " with " + to->toString();
    }
    int numVars() override {
        return 2;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class OrPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> to;
    std::unique_ptr<DataType> from;
public:
    OrPartialInstruction(std::unique_ptr<DataType> to, std::unique_ptr<DataType> from) : to(std::move(to)), from(std::move(from)) {}

    std::string toString() override {
        return "or " + from->toString() + " with " + to->toString();
    }
    int numVars() override {
        return 2;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class XorPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> to;
    std::unique_ptr<DataType> from;
public:
    XorPartialInstruction(std::unique_ptr<DataType> to, std::unique_ptr<DataType> from) : to(std::move(to)), from(std::move(from)) {}

    std::string toString() override {
        return "xor " + from->toString() + " with " + to->toString();
    }
    int numVars() override {
        return 2;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class IncrementPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> val;
public:
    IncrementPartialInstruction(std::unique_ptr<DataType> val) :val(std::move(val)) {}

    std::string toString() override {
        return "increment "+val->toString();
    }
    int numVars() override {
        return 1;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class DecrementPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> val;
public:
    DecrementPartialInstruction(std::unique_ptr<DataType> val) :val(std::move(val)) {}

    std::string toString() override {
        return "decrement "+val->toString();
    }
    int numVars() override {
        return 1;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class NegatePartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> val;
public:
    NegatePartialInstruction(std::unique_ptr<DataType> val) :val(std::move(val)) {}

    std::string toString() override {
        return "negate "+val->toString();
    }
    int numVars() override {
        return 1;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class BlockPartialInstruction : public PartialInstruction {
    std::vector<std::unique_ptr<PartialInstruction>> internalInstructions;
    std::string name;
    std::string endJmp;
    std::vector<std::string> localVariables;
    std::vector<std::string> paramVars;
public:
    BlockPartialInstruction(std::vector<std::unique_ptr<PartialInstruction>> content, std::string name, std::string endJmpLbl, const std::vector<std::string> &localVars, std::vector<std::string> paramVars) : internalInstructions(std::move(content)), name(std::move(name)), endJmp(std::move(endJmpLbl)), localVariables(localVars), paramVars(std::move(paramVars)) {}
    std::string toString() override;
    int numVars() override;
    std::unique_ptr<DataType>& getVariable(int vn) override;
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
    void validatFunctionCalls(std::vector<UserFunctionData> &functionNames) override;
    std::vector<std::string> getLocalVarScope(int vn, std::vector<std::string> &outerVarNames) override;
};

class FunctionCallPartialInstruction : public PartialInstruction {
    std::string name;
    std::unique_ptr<DataType> ignore;//its null just so there is a valid def for get variable
    int numberOfProvidedArgs;
public:
    FunctionCallPartialInstruction(std::string name, int numberOfProvidedArgs): name(std::move(name)), numberOfProvidedArgs(numberOfProvidedArgs) {}
    std::string toString() override {
        return "function call to " + name;
    }
    int numVars() override {
        return 0;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override {
        return ignore;
    }
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
    void validatFunctionCalls(std::vector<UserFunctionData> &functionNames) override;
};

class TrapPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> ignore;//its null just so there is a valid def for get variable
public:
  int numVars() override {
      return 0;
  }
    std::unique_ptr<DataType>& getVariable(int vn) override {
      return ignore;
  }
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
    std::string toString() override {
        return "trap";
    }
};

class StackPushPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> val;
    int existingStackOffset;
public:
    StackPushPartialInstruction(std::unique_ptr<DataType> val,int existingStackOffset) : val(std::move(val)), existingStackOffset(existingStackOffset) {}
    std::string toString() override {
        return "push " + val->toString();
    }
    int numVars() override {
        return 1;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override {
        return val;
    }
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class StackPopPartialInstruction : public PartialInstruction {
    std::unique_ptr<DataType> val;
    int existingStackOffset;
public:
    StackPopPartialInstruction(std::unique_ptr<DataType> val, int existingStackOffset) : val(std::move(val)), existingStackOffset(existingStackOffset) {}
    std::string toString() override {
        return "pop " + val->toString();
    }
    int numVars() override {
        return 1;
    }
    std::unique_ptr<DataType>& getVariable(int vn) override {
        return val;
    }
    std::vector<std::unique_ptr<FinishedInstruction>> assemble(RegisterResolver &resolver) override;
};

class HighLevelConstruct {
public:
    virtual ~HighLevelConstruct() = default;
    virtual std::vector<std::unique_ptr<PartialInstruction>> expand() = 0;
    virtual std::string toString() = 0;
};


/**parse a parameter into a datatype
 * @param value The trimmed string passed into something as a parameter
 * @return a unique pointer to a representation of the datatype
 */
inline std::unique_ptr<DataType> parseDataType(const std::string& value) {
    //get 0 out of the way first
    if (value.size() == 1 && value[0] == '0') {
        return std::make_unique<ZeroDataType>();
    }
    if (charIsNumber(value[0])) {//check for immediate
        if (!stringIsNumber(value)) {
            throw std::runtime_error("Error parsing data type: variables can not start with numbers");
        }
        int numberValue = std::stoi(value);
        return std::make_unique<ImmediateDataType>(numberValue);
    }
    //its a variable, make sure there is no whitespace in the name
    if (value.find_first_of(WHITESPACE) != std::string::npos) {
        throw std::runtime_error("Error parsing data type: variables can not contain with whitespace");
    }
    return std::make_unique<VariableDataType>(value);
}

//defined in main
std::unique_ptr<HighLevelConstruct> parseFileLine(const std::string& line, std::ifstream& file, int &lineNumber, std::vector<std::string> &localVars);
