#include "common.hpp"

#include <iostream>

std::string VariableDataType::asAsm() {
    // ReSharper disable once CppDFAConstantConditions
    if (!resolved) {
        throw std::runtime_error("Attempted to assemble unresolved variable: "+varName);
    }
    // ReSharper disable once CppDFAUnreachableCode
    if (stackVar) {
        //add 1 to the stack offset as the value of sp if the next stack value to be used not what we are refrencing
        return "[sp+"+std::to_string(offset+1)+"]";
    } else {
        if (varName.empty()) {
            throw std::runtime_error("Attempted to assemble empty variable");
        }
        return "["+varName+"]";
    }
}

std::string FinishedInstruction::produce() {
    std::string result;
    if (label) {
        result += "!" + operation;
    } else {
        result = "\t";
        result += operation;
        if (operands > 0) {
            result +=" ";
            result += op1;
            if (operands > 1) {
                result += ", ";
                result += op2;
            }
        }
    }
    //add comment here
    if (!comment.empty()) {
        result += "\t; "+comment;
    }
    return result;
}

std::string StackModificationAccountingFinishedInstruction::produce() {
    std::string op1Val = op1;
    if (op1Val.find("[sp+") != std::string::npos) {
        size_t endInd = op1Val.find_first_of(']');
        std::string numVal = op1Val.substr(4,endInd+1-4);
        int spof = std::stoi(numVal);
        spof +=stackOffset;
        op1Val = "[sp+"+std::to_string(spof)+"]";

    }
    std::string op2Val = op2;
    if (op2Val.find("[sp+") != std::string::npos) {
        size_t endInd = op2Val.find_first_of(']');
        std::string numVal = op2Val.substr(4,endInd+1-4);
        int spof = std::stoi(numVal);
        spof +=stackOffset;
        op2Val = "[sp+"+std::to_string(spof)+"]";

    }

    std::string result;
    if (label) {
        result += "!" + operation;
    } else {
        result = "\t";
        result += operation;
        if (operands > 0) {
            result +=" ";
            result += op1Val;
            if (operands > 1) {
                result += ", ";
                result += op2Val;
            }
        }
    }
    if (!comment.empty()) {
        result += "\t; "+comment;
    }
    //add comment here
    return result;
}

std::string RegisterResolver::resolve(std::unique_ptr<DataType> &data, std::vector<std::unique_ptr<FinishedInstruction>> &finishedInstructions, bool wrightOp,bool overwrite,int existingStackOffset) {
    if (overwrite && !wrightOp) {
        throw std::runtime_error("Resolver override flag set on a non wright op");
    }

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
    int imValue = 0;//tha value of an immediate but also the offset for a stack var
    std::string varName;
    if (lookForVar) {
        VariableDataType &vdt = *dynamic_cast<VariableDataType*>(data.get());
        varName = vdt.getVarName();
        imValue = vdt.getOffset();
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
        //set the register name
        outputRegister[1] += static_cast<char>(lruIndex); // NOLINT(*-narrowing-conversions)
        //if the reg is dirty then save the current value
        if (registers[lruIndex].dirty) {
            bool stack = false;
            for (auto& var : localVars) {
                if (var== registers[lruIndex].varName) {
                    stack = true;
                    break;
                }
            }
            if (!stack) {
                for (auto& var : paramVars) {
                    if (var == registers[lruIndex].varName) {
                        stack = true;
                        break;
                    }
                }
            }
            if (stack) {
                if (existingStackOffset != 0) {
                    finishedInstructions.emplace_back(std::make_unique<StackModificationAccountingFinishedInstruction>("str",2,"[sp"+std::to_string(registers[lruIndex].imValue)+"]",outputRegister,existingStackOffset,"save "+registers[lruIndex].varName+" to memory"));
                } else {
                    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"[sp+"+std::to_string(registers[lruIndex].imValue)+"]",outputRegister,"save "+registers[lruIndex].varName+" to memory"));
                }
                // std::cout << registers[lruIndex].imValue << std::endl;
                if (registers[lruIndex].imValue >= localVars.size()) {
                    FinishedInstruction * endPtr = finishedInstructions.back().get();
                    partiallyResolvedStackVars.push_back(endPtr);//another microslop hallucinated error
                }
            } else {
                if (registers[lruIndex].varName.empty()) {
                    throw std::runtime_error("Attempted to store empty global var!");
                }
                finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"["+registers[lruIndex].varName+"]",outputRegister,"save "+registers[lruIndex].varName+" to memory"));
            }
        }
        //load the new value into the register
        if (!overwrite) {//if not overriding the value in the next instruction
            const std::string loadCommand = lookForVar ? "lod": "set";
            std::string commentName = lookForVar ? varName:std::to_string(imValue);
            if (existingStackOffset !=0) {
                finishedInstructions.emplace_back(std::make_unique<StackModificationAccountingFinishedInstruction>(loadCommand,2,outputRegister,data->asAsm(),existingStackOffset, "load "+commentName +" into "+ outputRegister));
            } else {
                finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>(loadCommand,2,outputRegister,data->asAsm(),"load "+commentName +" into "+ outputRegister));
            }
            //check if it is extra stack and store the index for later correction
            if (lookForVar) {
                for (auto& var : paramVars) {
                    if (var == varName) {//if it is an extra stack var
                        FinishedInstruction * endIndex = finishedInstructions.back().get();
                        partiallyResolvedStackVars.push_back(endIndex);// add this instruction to the list
                        break;
                    }
                }
            }
        }

        //reset this reg lru
        //set the dirty bit correctly and all the other register things
        registers[lruIndex].lru =0;
        registers[lruIndex].dirty = wrightOp;
        registers[lruIndex].immediate = !lookForVar;
        registersUsed[lruIndex] = true;
        if (lookForVar) {
            registers[lruIndex].varName = varName;
            registers[lruIndex].imValue = imValue; //set stack offset here for local vars
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

int RegisterResolver::backupRegisters(std::vector<std::unique_ptr<FinishedInstruction>> &finishedInstructions) {
    //assuming all globals have been flushed
    int uregCnt = 0;
    for (size_t i = 0; i < registers.size(); i++) {
        if (registersUsed[i]) {//this register has been used, back it up!
            std::string regName = "rA";
            regName[1] += static_cast<char>(i); // NOLINT(*-narrowing-conversions)
            finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("psh",1,regName,"","backup "+regName+" to the stack"));
            uregCnt++;
        }
    }
    return uregCnt;
}

void RegisterResolver::restoreRegisters(std::vector<std::unique_ptr<FinishedInstruction>> &finishedInstructions) {
    for (int i = static_cast<int>(registers.size()) - 1; i >= 0; i--) {
        if (registersUsed[i]) {
            std::string regName = "rA";
            regName[1] += static_cast<char>(i); // NOLINT(*-narrowing-conversions)
            finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("pop",1,regName,"","restore "+regName+" from the stack"));
        }
    }
}

void RegisterResolver::flushGlobalVars(std::vector<std::unique_ptr<FinishedInstruction>> &finishedInstructions) const {
    for (size_t i = 0; i < registers.size(); i++) {
        if (!registers[i].varName.empty()){
            if (registers[i].dirty) {
                bool stack = false;
                for (auto& var : localVars) {
                    if (var== registers[i].varName) {
                        stack = true;
                        break;
                    }
                }
                for (auto& var : paramVars) {
                    if (var== registers[i].varName) {
                        stack = true;
                        break;
                    }
                }
                if (!stack) {
                    std::string outputRegister = "rA";
                    outputRegister[1] += static_cast<char>(i); // NOLINT(*-narrowing-conversions)
                    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"["+registers[i].varName+"]",outputRegister,"flush "+registers[i].varName+" to memory"));
                    //clear this reg
                    //make this as an empty reg
                    registers[i].varName = "";
                    registers[i].lru = 10000000;
                    registers[i].imValue =0;
                    registers[i].dirty = false;
                }
            }

        }
    }
}

void RegisterResolver::correctExtraStackVars(int numRegsUsed) const {
    int offsetBy = numRegsUsed + 1;
    //what is this has not elements
    for (auto instruction: partiallyResolvedStackVars) {
        std::string op1Val = instruction->op1;
        if (op1Val.find("[sp+") != std::string::npos) {
            size_t endInd = op1Val.find_first_of(']');
            std::string numVal = op1Val.substr(4,endInd+1-4);
            int spof = std::stoi(numVal);
            if (spof >= localVars.size()+1) {//+1 account for the fact that the stack pointer points to the next bit of memory that will be used
                spof +=offsetBy;
                instruction->op1 = "[sp+"+std::to_string(spof)+"]";
            }
        }
        std::string op2Val = instruction->op2;
        if (op2Val.find("[sp+") != std::string::npos) {//+1 account for the fact that the stack pointer points to the next bit of memory that will be used
            size_t endInd = op2Val.find_first_of(']');
            std::string numVal = op2Val.substr(4,endInd+1-4);
            int spof = std::stoi(numVal);
            if (spof >= localVars.size()+1) {
                spof +=offsetBy;
                instruction->op2 = "[sp+"+std::to_string(spof)+"]";
            }
        }
    }
}

void RegisterResolver::saveAllDirtyRegisters(std::vector<std::unique_ptr<FinishedInstruction>> &finishedInstructions) {
    for (size_t i = 0; i < registers.size(); i++) {
        if (registers[i].dirty) {
            bool stack = false;
            for (auto& var : localVars) {
                if (var== registers[i].varName) {
                    stack = true;
                    break;
                }
            }
            for (auto& var : paramVars) {
                if (var== registers[i].varName) {
                    stack = true;
                    break;
                }
            }
            registers[i].dirty = false;
            std::string outputRegister = "rA";
            outputRegister[1] += static_cast<char>(i); // NOLINT(*-narrowing-conversions)
            if (!stack) {
                finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"["+registers[i].varName+"]",outputRegister,"save "+registers[i].varName+" to memory"));
            } else {
                finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"[sp+"+std::to_string(registers[i].imValue)+"]",outputRegister,"save "+registers[i].varName+" to memory"));
                if (registers[i].imValue >= localVars.size()) {
                    FinishedInstruction * endPtr = finishedInstructions.back().get();
                    partiallyResolvedStackVars.push_back(endPtr);
                }
            }
        }
    }
}

void RegisterResolver::forceUpdateRegister(std::string &reg,std::vector<std::unique_ptr<FinishedInstruction>> &finishedInstructions, std::unique_ptr<DataType> &data) {
    int regNum = reg[1] - 'A';
    bool lookForVar = data->isVariable();
    if (registers[regNum].dirty) {//save this register if necessary
        bool stack = false;
        for (auto& var : localVars) {
            if (var== registers[regNum].varName) {
                stack = true;
                break;
            }
        }
        if (!stack) {
            for (auto& var : paramVars) {
                if (var == registers[regNum].varName) {
                    stack = true;
                    break;
                }
            }
        }
        if (stack) {
            finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"[sp+"+std::to_string(registers[regNum].imValue)+"]",reg,"save "+registers[regNum].varName+" to memory"));

            // std::cout << registers[lruIndex].imValue << std::endl;
            if (registers[regNum].imValue >= localVars.size()) {
                FinishedInstruction * endPtr = finishedInstructions.back().get();
                partiallyResolvedStackVars.push_back(endPtr);//another microslop hallucinated error
            }
        } else {
            if (registers[regNum].varName.empty()) {
                throw std::runtime_error("Attempted to store empty global var!");
            }
            finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"["+registers[regNum].varName+"]",reg,"save "+registers[regNum].varName+" to memory"));
        }
    } //end of save dirty
    int imValue = 0;//tha value of an immediate but also the offset for a stack var
    std::string varName;
    if (lookForVar) {
        VariableDataType &vdt = *dynamic_cast<VariableDataType*>(data.get());
        varName = vdt.getVarName();
        imValue = vdt.getOffset();
    } else {
        ImmediateDataType &idt = *dynamic_cast<ImmediateDataType*>(data.get());
        imValue = idt.getValue();
    }
    const std::string loadCommand = lookForVar ? "lod": "set";
    std::string commentName = lookForVar ? varName:std::to_string(imValue);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>(loadCommand,2,reg,data->asAsm(),"load "+commentName +" into "+ reg));

    //check if it is extra stack and store the index for later correction
    if (lookForVar) {
        for (auto& var : paramVars) {
            if (var == varName) {//if it is an extra stack var
                FinishedInstruction * endIndex = finishedInstructions.back().get();
                partiallyResolvedStackVars.push_back(endIndex);// add this instruction to the list
                break;
            }
        }
    }
}

std::vector<std::unique_ptr<FinishedInstruction>> DirectStorPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string inputReg = resolver.resolve(value,finishedInstructions,false,false);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,storeTo,inputReg));
    return finishedInstructions;
}

std::unique_ptr<DataType> & VariableAssignPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<std::unique_ptr<FinishedInstruction>> VariableAssignPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!to->isVariable()) {
        throw std::runtime_error("Variable assignment must have a varable on the left hand side");
    }
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true,true);
    auto * o1v = dynamic_cast<VariableDataType*>(to.get());
    std::string op1comment = o1v->getVarName();

    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    std::string op2Comment;
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(from.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
        op2Comment = from->toString();
    }

    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("set",2,op1Reg,op2Reg,"set "+op1comment+" to "+op2Comment));
    return finishedInstructions;
}

std::unique_ptr<DataType> & AddPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<std::unique_ptr<FinishedInstruction>> AddPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!to->isVariable()) {
        throw std::runtime_error("add operation left hand side must be a variable");
    }
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true,false);
    auto * o1v = dynamic_cast<VariableDataType*>(to.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    std::string op2Comment;
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(from.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
        op2Comment = from->toString();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("add",2,op1Reg,op2Reg,"add "+op2Comment+" to "+op1comment));
    return finishedInstructions;
}

std::unique_ptr<DataType> & SubtractPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<std::unique_ptr<FinishedInstruction>> SubtractPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!to->isVariable()) {
        throw std::runtime_error("subtract operation left hand side must be a variable");
    }
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true,false);
    auto * o1v = dynamic_cast<VariableDataType*>(to.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    std::string op2Comment;
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(from.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
        op2Comment = from->toString();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("sub",2,op1Reg,op2Reg,"subtract "+op2Comment+" from "+op1comment));
    return finishedInstructions;
}

std::unique_ptr<DataType> & MultiplyPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<std::unique_ptr<FinishedInstruction>> MultiplyPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!to->isVariable()) {
        throw std::runtime_error("multiply operation left hand side must be a variable");
    }
    auto * o1v = dynamic_cast<VariableDataType*>(to.get());
    std::string op1comment = o1v->getVarName();
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true,false);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    std::string op2Comment;
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(from.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
        op2Comment = from->toString();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("mpy",2,op1Reg,op2Reg,"multiply "+op2Comment+" with "+op1comment));
    return finishedInstructions;
}

std::unique_ptr<DataType> & DividePartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<std::unique_ptr<FinishedInstruction>> DividePartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!to->isVariable()) {
        throw std::runtime_error("divide operation left hand side must be a variable");
    }
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true,false);
    auto * o1v = dynamic_cast<VariableDataType*>(to.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    std::string op2Comment;
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(from.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
        op2Comment = from->toString();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("div",2,op1Reg,op2Reg,"divide "+op1comment+" by "+op2Comment));
    return finishedInstructions;
}

std::unique_ptr<DataType> & ModulusPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<std::unique_ptr<FinishedInstruction>> ModulusPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!to->isVariable()) {
        throw std::runtime_error("mod operation left hand side must be a variable");
    }
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true,false);
    auto * o1v = dynamic_cast<VariableDataType*>(to.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg;
    std::string op2Comment;
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(from.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
        op2Comment = from->toString();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("mod",2,op1Reg,op2Reg, "mod "+op1comment+" by "+op2Comment));
    return finishedInstructions;
}

std::unique_ptr<DataType> & AndPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<std::unique_ptr<FinishedInstruction>> AndPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!to->isVariable()) {
        throw std::runtime_error("and operation left hand side must be a variable");
    }
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true,false);
    auto * o1v = dynamic_cast<VariableDataType*>(to.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg;
    std::string op2Comment;
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(from.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
        op2Comment = from->toString();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("and",2,op1Reg,op2Reg,"and "+op1comment+" with "+op2Comment));
    return finishedInstructions;
}

std::unique_ptr<DataType> & OrPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<std::unique_ptr<FinishedInstruction>> OrPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!to->isVariable()) {
        throw std::runtime_error("or operation left hand side must be a variable");
    }
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true,false);
    auto * o1v = dynamic_cast<VariableDataType*>(to.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg;
    std::string op2Comment;
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(from.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
        op2Comment = from->toString();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("or",2,op1Reg,op2Reg,"or "+op1comment+" with "+op2Comment));
    return finishedInstructions;
}

std::unique_ptr<DataType> & XorPartialInstruction::getVariable(int vn) {
    if (vn==0) {
        return to;
    }
    return from;
}

std::vector<std::unique_ptr<FinishedInstruction>> XorPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!to->isVariable()) {
        throw std::runtime_error("xor operation left hand side must be a variable");
    }
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true,false);
    auto * o1v = dynamic_cast<VariableDataType*>(to.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg;
    std::string op2Comment;
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(from.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
        op2Comment = from->toString();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("xor",2,op1Reg,op2Reg, "xor "+op1comment+" with "+op2Comment));
    return finishedInstructions;
}

std::unique_ptr<DataType> & IncrementPartialInstruction::getVariable(int vn) {
    return val;
}

std::vector<std::unique_ptr<FinishedInstruction>> IncrementPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg = resolver.resolve(val,finishedInstructions,true,false);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("inc",1,op1Reg,"",false));
    return finishedInstructions;
}

std::unique_ptr<DataType> & DecrementPartialInstruction::getVariable(int vn) {
    return val;
}

std::vector<std::unique_ptr<FinishedInstruction>> DecrementPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg = resolver.resolve(val,finishedInstructions,true,false);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("dec",1,op1Reg,""));
    return finishedInstructions;
}

std::unique_ptr<DataType> & NegatePartialInstruction::getVariable(int vn) {
    return val;
}

std::vector<std::unique_ptr<FinishedInstruction>> NegatePartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg = resolver.resolve(val,finishedInstructions,true,false);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("neg",1,op1Reg,""));
    return finishedInstructions;
}

std::string BlockPartialInstruction::toString() {
    return "block"; //TODO make this print more stuf
}

int BlockPartialInstruction::numVars() {
    int total = 0;
    for (auto & inst:internalInstructions) {
        total += inst->numVars();
    }
    if (endLoopCondition != nullptr) {
        total += endLoopCondition->numVars();

    }
    if (returnValue != nullptr) {
        total ++;
    }
    return total;
}

std::unique_ptr<DataType> & BlockPartialInstruction::getVariable(int vn) {
    int rt =0;//running total
    for (auto & inst:internalInstructions) {
        int ni = inst->numVars();
        if (rt+ni > vn) {//if adding this instructions vars to the total pust the one we want out of reach
            return inst->getVariable(vn-rt);//its in this one
        }
        rt += ni;
    }
    if (endLoopCondition != nullptr) {
        int ni = endLoopCondition->numVars();
        if (rt+ni > vn) {
            return endLoopCondition->getVariable(vn-rt);
        }
        rt += ni;
    }
    if (returnValue != nullptr && rt == vn) {
        return returnValue;
    }
    return internalInstructions[0]->getVariable(0);//fail safe
}

std::vector<std::unique_ptr<FinishedInstruction>> BlockPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<Register> registers;
    for (int i=0;i<5;i++) {
        registers.emplace_back(i);
    }
    std::vector<std::unique_ptr<FinishedInstruction>> finalInstructions;
    RegisterResolver blockResolver(registers,localVariables,paramVars);
    std::vector<std::unique_ptr<FinishedInstruction>> inProgressInstructions;
    std::string returnValueReg;
    bool returningValue = returnValue != nullptr;
    //get the assembled form of all the content
    for (auto &instruction: internalInstructions) {
        std::vector<std::unique_ptr<FinishedInstruction>> tmp = instruction->assemble(blockResolver);
        for (auto &instInfo : tmp) {
            inProgressInstructions.push_back(std::move(instInfo));
        }
    }
    if (returningValue) {
        returnValueReg = blockResolver.resolve(returnValue,inProgressInstructions,false,false);
    }

    if (endLoopCondition != nullptr) {//if this is a loop and there is a condition for the end, resolve its vars before ant register backups
        //save all used registers
        blockResolver.saveAllDirtyRegisters(inProgressInstructions);
        std::vector<std::unique_ptr<FinishedInstruction>> tmp = endLoopCondition->assemble(blockResolver);
        for (auto &instInfo : tmp) {
            inProgressInstructions.emplace_back(std::move(instInfo));
        }
    }

    //add label if label before register backup
    if (!backupPreLabel) {
        finalInstructions.emplace_back(std::make_unique<FinishedInstruction>(name,0,"","",true));
    }
    //pre pend all the register and stack var prep
    int numRegistersUsed = blockResolver.backupRegisters(finalInstructions);
    //add label if back registers before label
    if (backupPreLabel) {
        //aldo add the local vars here because theese need to be right before the label in a loop
        for (int i=0;i<localVariables.size();i++) {
            finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("psh",1,"rz","",false,"push space for "+localVariables[localVariables.size()-1-i]+" onto the stack"));
        }
        finalInstructions.emplace_back(std::make_unique<FinishedInstruction>(name,0,"","",true));
    }

    //account for params stack offset
    blockResolver.correctExtraStackVars(numRegistersUsed);
    //stack var are already pushed at this point by the system in the level above this


    //add all the computed instructions
    for (auto &instruction: inProgressInstructions) {
        finalInstructions.push_back(std::move(instruction));
    }

    //store the return value
    if (returningValue) {
        //calculate the location of the return value on the stack
        //1 return address + 1 offset from the stack pointer + number of local variables + numbers of parameters + number of registers used
        int returnStackPos = static_cast<int>(2 + localVariables.size() + paramVars.size() + numRegistersUsed);
        finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"[sp+"+std::to_string(returnStackPos)+"]",returnValueReg,false));
    }

    //flush changed global vars
    blockResolver.flushGlobalVars(finalInstructions);


    if (backupPreLabel) {//if backup pre label then do not clean up the stack until after the label
        if (endJmp.empty()) {
            finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("ret",0,"","",false));
        } else {
            finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("jmp",1,"!"+endJmp,"",false));
        }
        finalInstructions.emplace_back(std::make_unique<FinishedInstruction>(cleanupLabel,0,"","",true));
    }
    //add the stack cleanup
    //pop stack vars here
    for (size_t i=0;i<localVariables.size();i++) {
        finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("pop",1,"rz","",false));
    }
    blockResolver.restoreRegisters(finalInstructions);

    //add final jump for return if the label goes after the restore
    if (!backupPreLabel) {
        if (endJmp.empty()) {
            finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("ret",0,"","",false));
        } else {
            finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("jmp",1,"!"+endJmp,"",false));
        }
    }

    return finalInstructions;
}

void BlockPartialInstruction::validatFunctionCalls(std::vector<UserFunctionData> &functionNames) {
    for (const auto & ir:internalInstructions) {
        ir->validatFunctionCalls(functionNames);
    }
}

std::vector<std::string> BlockPartialInstruction::getLocalVarScope(int vn, std::vector<std::string> &outerVarNames) {
    std::vector<std::string> localVarsAndParams(localVariables);
    for (const auto & pv:paramVars) {
        localVarsAndParams.push_back(pv);
    }
    int rt =0;//running total
    for (auto & inst:internalInstructions) {
        int ni = inst->numVars();
        if (rt+ni > vn) {//if adding this instructions vars to the total pust the one we want out of reach
            return inst->getLocalVarScope(vn-rt,localVarsAndParams);//its in this one
        }
        rt += ni;
    }
    if (endLoopCondition != nullptr) {
        int ni = endLoopCondition->numVars();
        if (rt+ni > vn) {
            return endLoopCondition->getLocalVarScope(vn-rt,localVarsAndParams);
        }
        rt += ni;
    }
    if (returnValue != nullptr && rt == vn) {//the variable used int eh return statement
        return localVarsAndParams;
    }

    return PartialInstruction::getLocalVarScope(vn, outerVarNames);
}

std::vector<std::unique_ptr<FinishedInstruction>> FunctionCallPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    resolver.flushGlobalVars(finishedInstructions);
    //note: pushing of params and return values should be done in other partial instructions created by the high level operation
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("cal",1,"!"+name,"",false));
    return finishedInstructions;
}

void FunctionCallPartialInstruction::validatFunctionCalls(std::vector<UserFunctionData> &functionNames) {
    for (auto &fname:functionNames) {
        if (fname.name == this->name) {
            if (numberOfProvidedArgs == fname.numberOfParameters) {
                if (!fname.returnsData && expectsReturnValue) {
                    throw std::runtime_error("Function "+fname.name+" does not have a return value.");
                }
                return;
            } else {
                throw std::runtime_error("Attempted to call function \""+fname.name+"\" with incorrect number of arguments, expected: "+std::to_string(fname.numberOfParameters)+" got: "+std::to_string(numberOfProvidedArgs) );
            }
        }
    }
    std::cerr << functionNames.size() << " functions were declared." << std::endl;
    for (auto &fname:functionNames) {
        std::cerr << fname.name<<std::endl;
    }
    throw std::runtime_error("Function not found: "+name+ " "+std::to_string(name.size()));
}

int trapCnt = 12;

std::vector<std::unique_ptr<FinishedInstruction>> TrapPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("stupidVerlyLongTrapLabel_"+std::to_string(trapCnt),0,"","",true));
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("jmp",1,"!stupidVerlyLongTrapLabel_"+std::to_string(trapCnt),"",false));
    trapCnt++;
    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> StackPushPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (val->isVariable()) {//if it is a variable the resolve it
        op1Reg = resolver.resolve(val,finishedInstructions,false,false,existingStackOffset);
    } else {//if it is not a variable it does not need to be resolved for this
        op1Reg = val->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<StackModificationAccountingFinishedInstruction>("psh",1,op1Reg,"",existingStackOffset,comment));
    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> StackPopPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (val->isVariable()) {//if it is a variable the resolve it
        op1Reg = resolver.resolve(val,finishedInstructions,true,false,existingStackOffset);
    } else {//if it is not a variable it does not need to be resolved for this
        op1Reg = val->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<StackModificationAccountingFinishedInstruction>("pop",1,op1Reg,"",existingStackOffset));
    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> JumpConditionPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg = resolver.resolve(data1,finishedInstructions,false,false);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (data2->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(data2,finishedInstructions,false,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = data2->asAsm();
    }
    //compare instruction
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("cmp",2,op1Reg,op2Reg,false));
    //jump time!
    std::string jumpType = conditionToOppositeJump(condition);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>(jumpType,1,"!"+jumpTo,"",false));
    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> DelayPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg;
    if (data->isVariable()) {
        op1Reg = resolver.resolve(data,finishedInstructions,false,false);
    } else {
        op1Reg = data->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("dly",1,op1Reg,"",false));
    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> DirectLoadPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!loadTo->isVariable()) {
        throw std::runtime_error("Load instructions require a variable to store in");
    }
    std::string op1Reg = resolver.resolve(loadTo,finishedInstructions,true,true);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("lod",2,op1Reg,"["+loadFrom+"]",false));
    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> RightShiftPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!data->isVariable()) {
        throw std::runtime_error("right shift operation left hand side must be a variable");
    }
    std::string op1Reg = resolver.resolve(data,finishedInstructions,true,false);
    auto * o1v = dynamic_cast<VariableDataType*>(data.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg = resolver.resolve(amount,finishedInstructions,false,false);;
    std::string op2Comment;
    if (amount->isVariable()) {
        auto* vdt = dynamic_cast<VariableDataType*>(amount.get());
        op2Comment = vdt->getVarName();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("shf",2,op1Reg,op2Reg, "right shift "+op1comment+" by "+op2Comment));


    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> LeftShiftPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!data->isVariable()) {
        throw std::runtime_error("left shift operation left hand side must be a variable");
    }
    std::string op1Reg = resolver.resolve(data,finishedInstructions,true,false);
    auto * o1v = dynamic_cast<VariableDataType*>(data.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg = resolver.resolve(amount,finishedInstructions,false,false);
    std::string op2Comment;
    if (amount->isVariable()) { // just for the comment at this point
        auto* vdt = dynamic_cast<VariableDataType*>(amount.get());
        op2Comment = vdt->getVarName();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("neg",1,op2Reg,"",false,"negate "+op2Comment));
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("shf",2,op1Reg,op2Reg, "left shift "+op1comment+" by "+op2Comment));
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("neg",1,op2Reg,"",false,"put "+op2Comment+" back to what it is suposted to be so the register cache is not broken"));

    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> PrintStringPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::unique_ptr<DataType> currentImmediate = std::make_unique<ImmediateDataType>(toPrint[0]);
    std::string comment = "print X";
    comment[6] = toPrint[0];
    std::string useReg = resolver.resolve(currentImmediate,finishedInstructions,false,false);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"["+ariteAddr+"]",useReg,false,comment));
    for (size_t i = 1; i < toPrint.size(); i++) {
        //update the cache with the value
        char strChar = toPrint[i];
        comment = "print X";
        comment[6] = strChar;
        currentImmediate = std::make_unique<ImmediateDataType>(strChar);
        //resolve
        if (strChar != toPrint[i-1]) {//only update what is in the register if the current char is different from the last one
            resolver.forceUpdateRegister(useReg,finishedInstructions,currentImmediate);
        }
        finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"["+ariteAddr+"]",useReg,false,comment));
    }
    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> DirectMemoryReadPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    if (!returnTo->isVariable()) {
        throw std::runtime_error("Direct memory read operation must store the rsult in a varable variable");
    }
    std::string op1Reg = resolver.resolve(returnTo,finishedInstructions,true,true);
    auto * o1v = dynamic_cast<VariableDataType*>(returnTo.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg;
    std::string op2Comment;
    if (loadFrom->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(loadFrom,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(loadFrom.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = loadFrom->asAsm();
        op2Comment = loadFrom->toString();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("lod",2,op1Reg,"["+op2Reg+"]",false,"read from memory address"+ op2Comment +" into "+op2Comment));
    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> DirectMemoryWritePartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg = resolver.resolve(value,finishedInstructions,true,true);
    auto * o1v = dynamic_cast<VariableDataType*>(value.get());
    std::string op1comment = o1v->getVarName();
    std::string op2Reg;
    std::string op2Comment;
    if (writeTo->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(writeTo,finishedInstructions,false,false);
        auto* vdt = dynamic_cast<VariableDataType*>(writeTo.get());
        op2Comment = vdt->getVarName();
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = writeTo->asAsm();
        op2Comment = writeTo->toString();
    }
    //I know the op numbers are backwards here. I do not care!
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,op2Reg,op1Reg,false,"write to memory address"+op2Comment + " the value in "+op1comment));
    return finishedInstructions;
}