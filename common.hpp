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
    std::string asAsm() override {
        if (!resolved) {
            throw std::runtime_error("Attempted to assemble unresolved variable: "+varName);
        }
        if (stackVar) {
            return "[SP+"+std::to_string(offset)+"]";
        } else {
            return "["+varName+"]";
        }
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
    int getValue() {
        return value;
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

struct FinishedInstruction {
    std::string operation;
    int operands;
    std::string op1;
    std::string op2;
    [[nodiscard]] std::string produce() const {
        std::string result = "\t";
        result += operation;
        if (operands > 0) {
            result +=" ";
            result += op1;
            if (operands > 1) {
                result += ", ";
                result += op2;
            }
        }
        //add comment here
        return result;
    }
};

class RegisterResolver {
    std::vector<Register> &registers;
    std::vector<std::string> &localVars;
    std::vector<bool> registersUsed;
public:
    RegisterResolver(std::vector<Register> &registers, std::vector<std::string> &localVars) :
    registers(registers), localVars(localVars) {
        registersUsed.resize(registers.size());
    }

    std::string resolve(std::unique_ptr<DataType>& data, std::vector<FinishedInstruction>& finishedInstructions, bool wrightOp) {
        //check what the data is
        if (!data->needsResolve()) {//if it does not need register caching,
            return data->asAsm();//just return what it is
        }

        //increment each registers lru
        for (Register& reg : registers) {
            reg++;
        }
        //look through the registers to see if the value is cached
        bool lookForVar = data->isVariable();
        int imValue = 0;
        std::string varName;
        if (lookForVar) {
            VariableDataType &vdt = *dynamic_cast<VariableDataType*>(data.get());
            varName = vdt.getVarName();
        } else {
            if (wrightOp) {
                throw std::runtime_error("Attempted to preform a wright op on an immediate!");
            }
            ImmediateDataType &idt = *dynamic_cast<ImmediateDataType*>(data.get());
            imValue = idt.getValue();
        }
        int foundIndex = -1;
        for (size_t i = 0; i < registers.size(); i++) {
            //check if the register contains the value we want,
            if (registers[i].immediate && !lookForVar && registers[i].imValue == imValue ) {
                foundIndex = static_cast<int>(i);
                break;
            } else if (!registers[i].immediate && lookForVar &&registers[i].varName == varName) {
                foundIndex = static_cast<int>(i);
                break;
            }
        }

        std::string outputRegister = "rA";

        if (foundIndex == -1) { //if the value was not found in the register cache
            //eviction process

            //find the leased recently used (higer lru value) register
            size_t lruIndex=0;
            int highestLruValue=0;
            for (size_t i = 0; i < registers.size(); i++) {
                if (registers[i].lru > highestLruValue) {
                    lruIndex = i;
                    highestLruValue = registers[i].lru;
                }
            }
            outputRegister[1] += static_cast<char>(lruIndex); // NOLINT(*-narrowing-conversions)
            //if the reg is dirty then save the current value
            if (registers[lruIndex].dirty) {
                //TODO check if it is a stack var, is fo then imValue is the offset
                finishedInstructions.push_back({"str",2,"["+registers[lruIndex].varName+"]",outputRegister});
            }
            //load the new value into the register
            const std::string loadCommand = lookForVar ? "lod": "set";
            finishedInstructions.push_back({loadCommand,2,outputRegister,data->asAsm()});

            //reset this reg lru
            //set the dirty bit correctly and all the other register things
            registers[lruIndex].lru =0;
            registers[lruIndex].dirty = wrightOp;
            registers[lruIndex].immediate = !lookForVar;
            if (lookForVar) {
                registers[lruIndex].varName = varName;
                registers[lruIndex].imValue = 0; //set stack offset here for local vars
            } else {
                registers[lruIndex].varName = "";
                registers[lruIndex].imValue = imValue;
            }
            //return the reg name

        } else { //if the value was found in the registers
            //no need to add additional instructions, just reset its LRU and return the register name
            registers[foundIndex].lru = 0;
            registers[foundIndex].dirty |= wrightOp;
            registersUsed[foundIndex] = true;
            outputRegister[1] += static_cast<char>(foundIndex); // NOLINT(*-narrowing-conversions)
        }
        return outputRegister;
    }
};


class PartialInstruction {
public:
    virtual ~PartialInstruction() =default;
    virtual std::string toString() = 0;
    virtual int numVars() = 0;
    virtual std::unique_ptr<DataType>& getVariable(int vn) = 0;
    virtual std::vector<FinishedInstruction> assemble(RegisterResolver &resolver) = 0;
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

    std::vector<FinishedInstruction> assemble(RegisterResolver &resolver) override {
        std::vector<FinishedInstruction> finishedInstructions;
        std::string inputReg = resolver.resolve(value,finishedInstructions,false);
        finishedInstructions.push_back({"str",2,storeTo,inputReg});
        return finishedInstructions;
    }
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
    std::unique_ptr<DataType>& getVariable(int vn) override {
        if (vn==0) {
            return to;
        }
        return from;
    }
    std::vector<FinishedInstruction> assemble(RegisterResolver &resolver) override {
        std::vector<FinishedInstruction> finishedInstructions;
        std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
        std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
        if (from->isVariable()) {//if it is a variable the resolve it
            op2Reg = resolver.resolve(from,finishedInstructions,false);
        } else {//if it is not a variable it does not need to be resolved for this
            op2Reg = from->asAsm();
        }
        finishedInstructions.push_back({"set",2,op1Reg,op2Reg});
        return finishedInstructions;
    }
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