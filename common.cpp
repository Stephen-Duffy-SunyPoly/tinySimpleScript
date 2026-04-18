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
    return result;
}

std::string StackModificationAccountingFinishedInstruction::produce() {
    std::string op1Val = op1;
    std::cout << operation << std::endl;
    std::cout << "o1 "<<op1Val<<" "<<op1Val.find_first_of("[sp+")<<std::endl;
    if (op1Val.find("[sp+") != std::string::npos) {
        size_t endInd = op1Val.find_first_of(']');
        std::string numVal = op1Val.substr(4,endInd+1-4);
        int spof = std::stoi(numVal);
        std:: cout << "pmsvP1: "<<spof <<"  "<<stackOffset<<std::endl;
        spof +=stackOffset;
        op1Val = "[sp+"+std::to_string(spof)+"]";

    }
    std::string op2Val = op2;
    std::cout << "o2 "<<op2Val<<" "<<op2Val.find_first_of("[sp+")<<std::endl;
    if (op2Val.find("[sp+") != std::string::npos) {
        size_t endInd = op2Val.find_first_of(']');
        std::string numVal = op2Val.substr(4,endInd+1-4);
        int spof = std::stoi(numVal);
        std:: cout << "pmsvP2: "<<spof <<"  "<<stackOffset<<std::endl;
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
    //add comment here
    return result;
}

std::string RegisterResolver::resolve(std::unique_ptr<DataType> &data, std::vector<std::unique_ptr<FinishedInstruction>> &finishedInstructions, bool wrightOp,int existingStackOffset) {
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
                    finishedInstructions.emplace_back(std::make_unique<StackModificationAccountingFinishedInstruction>("str",2,"[sp"+std::to_string(registers[lruIndex].imValue)+"]",outputRegister,existingStackOffset));
                } else {
                    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"[sp+"+std::to_string(registers[lruIndex].imValue)+"]",outputRegister));
                }
                std::cout << registers[lruIndex].imValue << std::endl;
                if (registers[lruIndex].imValue >= localVars.size()) {
                    FinishedInstruction * endPtr = finishedInstructions.back().get();
                    partiallyResolvedStackVars.push_back(endPtr);//another microslop hallucinated error
                }
            } else {
                finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"["+registers[lruIndex].varName+"]",outputRegister));
            }
        }
        //load the new value into the register
        const std::string loadCommand = lookForVar ? "lod": "set";
        if (existingStackOffset !=0) {
            finishedInstructions.emplace_back(std::make_unique<StackModificationAccountingFinishedInstruction>(loadCommand,2,outputRegister,data->asAsm(),existingStackOffset));
        } else {
            finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>(loadCommand,2,outputRegister,data->asAsm()));
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
            finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("psh",1,regName,""));
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
            finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("pop",1,regName,""));
        }
    }
}

void RegisterResolver::flushGlobalVars(std::vector<std::unique_ptr<FinishedInstruction>> &finishedInstructions) const {
    for (size_t i = 0; i < registers.size(); i++) {
        //TODO check this id a global var once we have a way to check that stuff
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
                    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("str",2,"["+registers[i].varName+"]",outputRegister));
                    //clear this reg
                    registers[i].dirty = false;
                }
            }
            //make this as an empty reg
            registers[i].varName = "";
            registers[i].lru = 10000000;
            registers[i].imValue =0;
        }
    }
}

void RegisterResolver::correctExtraStackVars(int numRegsUsed, std::vector<std::unique_ptr<FinishedInstruction>> &finishedInstructions) const {
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

std::vector<std::unique_ptr<FinishedInstruction>> DirectStorPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string inputReg = resolver.resolve(value,finishedInstructions,false);
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
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("set",2,op1Reg,op2Reg));
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
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("add",2,op1Reg,op2Reg));
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
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("sub",2,op1Reg,op2Reg));
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
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("mpy",2,op1Reg,op2Reg));
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
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("div",2,op1Reg,op2Reg));
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
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("mod",2,op1Reg,op2Reg));
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
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("and",2,op1Reg,op2Reg));
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
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("or",2,op1Reg,op2Reg));
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
    std::string op1Reg = resolver.resolve(to,finishedInstructions,true);
    std::string op2Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (from->isVariable()) {//if it is a variable the resolve it
        op2Reg = resolver.resolve(from,finishedInstructions,false);
    } else {//if it is not a variable it does not need to be resolved for this
        op2Reg = from->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("xor",2,op1Reg,op2Reg));
    return finishedInstructions;
}

std::unique_ptr<DataType> & IncrementPartialInstruction::getVariable(int vn) {
    return val;
}

std::vector<std::unique_ptr<FinishedInstruction>> IncrementPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg = resolver.resolve(val,finishedInstructions,true);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("inc",1,op1Reg,"",false));
    return finishedInstructions;
}

std::unique_ptr<DataType> & DecrementPartialInstruction::getVariable(int vn) {
    return val;
}

std::vector<std::unique_ptr<FinishedInstruction>> DecrementPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg = resolver.resolve(val,finishedInstructions,true);
    finishedInstructions.emplace_back(std::make_unique<FinishedInstruction>("dec",1,op1Reg,""));
    return finishedInstructions;
}

std::unique_ptr<DataType> & NegatePartialInstruction::getVariable(int vn) {
    return val;
}

std::vector<std::unique_ptr<FinishedInstruction>> NegatePartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg = resolver.resolve(val,finishedInstructions,true);
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
        returnValueReg = blockResolver.resolve(returnValue,inProgressInstructions,false);
    }
    //add label
    finalInstructions.emplace_back(std::make_unique<FinishedInstruction>(name,0,"","",true));
    //pre pend all the register and stack var prep
    int numRegistersUsed = blockResolver.backupRegisters(finalInstructions);

    //account for params stack offset
    blockResolver.correctExtraStackVars(numRegistersUsed,inProgressInstructions);
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

    //add the stack cleanup
    //pop stack vars here
    for (size_t i=0;i<localVariables.size();i++) {
        finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("pop",1,"rz","",false));
    }
    blockResolver.restoreRegisters(finalInstructions);

    //add final jump for return
    if (endJmp.empty()) {
        finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("ret",0,"","",false));
    } else {
        finalInstructions.emplace_back(std::make_unique<FinishedInstruction>("jmp",1,"!"+endJmp,"",false));
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
                return;
            } else {
                throw std::runtime_error("Attempted to call function \""+fname.name+"\" with incorrect number of arguments, expected: "+std::to_string(fname.numberOfParameters)+" got: "+std::to_string(numberOfProvidedArgs) );
            }
        }
    }
    std::cerr << functionNames.size() << " functions were declared." << std::endl;
    for (auto &fname:functionNames) {
        std::cerr << fname.name<< std::endl;
    }
    throw std::runtime_error("Function not found: "+name);
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
        op1Reg = resolver.resolve(val,finishedInstructions,false,existingStackOffset);
    } else {//if it is not a variable it does not need to be resolved for this
        op1Reg = val->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<StackModificationAccountingFinishedInstruction>("psh",1,op1Reg,"",existingStackOffset));
    return finishedInstructions;
}

std::vector<std::unique_ptr<FinishedInstruction>> StackPopPartialInstruction::assemble(RegisterResolver &resolver) {
    std::vector<std::unique_ptr<FinishedInstruction>> finishedInstructions;
    std::string op1Reg;// = resolver.resolve(from,finishedInstructions,false);
    if (val->isVariable()) {//if it is a variable the resolve it
        op1Reg = resolver.resolve(val,finishedInstructions,false,existingStackOffset);
    } else {//if it is not a variable it does not need to be resolved for this
        op1Reg = val->asAsm();
    }
    finishedInstructions.emplace_back(std::make_unique<StackModificationAccountingFinishedInstruction>("pop",1,op1Reg,"",existingStackOffset));
    return finishedInstructions;
}