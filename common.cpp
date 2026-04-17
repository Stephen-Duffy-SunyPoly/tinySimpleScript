#include "common.hpp"

std::string VariableDataType::asAsm() {
    // ReSharper disable once CppDFAConstantConditions
    if (!resolved) {
        throw std::runtime_error("Attempted to assemble unresolved variable: "+varName);
    }
    // ReSharper disable once CppDFAUnreachableCode
    if (stackVar) {
        return "[SP+"+std::to_string(offset)+"]";
    } else {
        return "["+varName+"]";
    }
}

std::string FinishedInstruction::produce() const {
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

std::string RegisterResolver::resolve(std::unique_ptr<DataType> &data,
    std::vector<FinishedInstruction> &finishedInstructions, bool wrightOp) {
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
        }
        if (!registers[i].immediate && lookForVar &&registers[i].varName == varName) {
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

std::vector<FinishedInstruction> DirectStorPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string inputReg = resolver.resolve(value,finishedInstructions,false);
    finishedInstructions.push_back({"str",2,storeTo,inputReg});
    return finishedInstructions;
}

std::unique_ptr<DataType> & VariableAssignPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<FinishedInstruction> VariableAssignPartialInstruction::assemble(RegisterResolver &resolver) {
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

std::unique_ptr<DataType> & AddPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<FinishedInstruction> AddPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.push_back({"add",2,op1Reg,op2Reg});
    return finishedInstructions;
}

std::unique_ptr<DataType> & SubtractPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<FinishedInstruction> SubtractPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.push_back({"sub",2,op1Reg,op2Reg});
    return finishedInstructions;
}

std::unique_ptr<DataType> & MultiplyPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<FinishedInstruction> MultiplyPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.push_back({"mpy",2,op1Reg,op2Reg});
    return finishedInstructions;
}

std::unique_ptr<DataType> & DividePartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<FinishedInstruction> DividePartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.push_back({"div",2,op1Reg,op2Reg});
    return finishedInstructions;
}

std::unique_ptr<DataType> & ModulusPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<FinishedInstruction> ModulusPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.push_back({"mod",2,op1Reg,op2Reg});
    return finishedInstructions;
}

std::unique_ptr<DataType> & AndPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<FinishedInstruction> AndPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.push_back({"and",2,op1Reg,op2Reg});
    return finishedInstructions;
}

std::unique_ptr<DataType> & OrPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<FinishedInstruction> OrPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.push_back({"or",2,op1Reg,op2Reg});
    return finishedInstructions;
}

std::unique_ptr<DataType> & XorPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<FinishedInstruction> XorPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.push_back({"xor",2,op1Reg,op2Reg});
    return finishedInstructions;
}

std::unique_ptr<DataType> & IncrementPartialInstruction::getVariable(int vn) {
    return val;
}

std::vector<FinishedInstruction> IncrementPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(val,finishedInstructions,true);
    finishedInstructions.push_back({"inc",1,op1Reg,{}});
    return finishedInstructions;
}

std::unique_ptr<DataType> & DecrementPartialInstruction::getVariable(int vn) {
    return val;
}

std::vector<FinishedInstruction> DecrementPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(val,finishedInstructions,true);
    finishedInstructions.push_back({"dec",1,op1Reg,{}});
    return finishedInstructions;
}

std::unique_ptr<DataType> & NegatePartialInstruction::getVariable(int vn) {
    return val;
}

std::vector<FinishedInstruction> NegatePartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<FinishedInstruction> finishedInstructions;
    std::string op1Reg = resolver.resolve(val,finishedInstructions,true);
    finishedInstructions.push_back({"neg",1,op1Reg,{}});
    return finishedInstructions;
}
