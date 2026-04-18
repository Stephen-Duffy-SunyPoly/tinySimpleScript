#include "commonHighLevel.hpp"

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>


VariableAssignment::VariableAssignment(const std::string &name, const std::string &value) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("Assignment operator left hand side must be a variable ");
    }
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> VariableAssignment::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<VariableAssignPartialInstruction>(std::move(var), std::move(value)));
    return instructions;
}

std::string VariableAssignment::toString() {
    return var->toString() +" = "+value->toString();
}

AddHighLevelOperation::AddHighLevelOperation(const std::string &name, const std::string &value) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("addition operator left hand side must be a variable");
    }
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> AddHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<AddPartialInstruction>(std::move(var), std::move(value)));
    return instructions;
}

std::string AddHighLevelOperation::toString() {
    return var->toString() +" += "+value->toString();
}

SubtractHighLevelOperation::SubtractHighLevelOperation(const std::string &name, const std::string &value) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("subtraction operator left hand side must be a variable");
    }
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> SubtractHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<SubtractPartialInstruction>(std::move(var), std::move(value)));
    return instructions;
}

std::string SubtractHighLevelOperation::toString() {
    return var->toString() +" -= "+value->toString();
}

MultiplyHighLevelOperation::MultiplyHighLevelOperation(const std::string &name, const std::string &value) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("multiplication operator left hand side must be a variable");
    }
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> MultiplyHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<MultiplyPartialInstruction>(std::move(var), std::move(value)));
    return instructions;
}

std::string MultiplyHighLevelOperation::toString() {
    return var->toString() +" *= "+value->toString();
}

DivideHighLevelOperation::DivideHighLevelOperation(const std::string &name, const std::string &value) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("division operator left hand side must be a variable");
    }
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> DivideHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DividePartialInstruction>(std::move(var), std::move(value)));
    return instructions;
}

std::string DivideHighLevelOperation::toString() {
    return var->toString() +" /= "+value->toString();
}

ModulusHighLevelOperation::ModulusHighLevelOperation(const std::string &name, const std::string &value) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("modulus operator left hand side must be a variable");
    }
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> ModulusHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<ModulusPartialInstruction>(std::move(var), std::move(value)));
    return instructions;
}

std::string ModulusHighLevelOperation::toString() {
    return var->toString() +" %= "+value->toString();
}

AndHighLevelOperation::AndHighLevelOperation(const std::string &name, const std::string &value) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("and operator left hand side must be a variable");
    }
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> AndHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<AndPartialInstruction>(std::move(var), std::move(value)));
    return instructions;
}

std::string AndHighLevelOperation::toString() {
    return var->toString() +" &= "+value->toString();
}

OrHighLevelOperation::OrHighLevelOperation(const std::string &name, const std::string &value) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("or operator left hand side must be a variable");
    }
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> OrHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<OrPartialInstruction>(std::move(var), std::move(value)));
    return instructions;
}

std::string OrHighLevelOperation::toString() {
    return var->toString() +" |= "+value->toString();
}

XorHighLevelOperation::XorHighLevelOperation(const std::string &name, const std::string &value) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("xor operator left hand side must be a variable");
    }
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> XorHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<XorPartialInstruction>(std::move(var), std::move(value)));
    return instructions;
}

std::string XorHighLevelOperation::toString() {
    return var->toString() +" ^= "+value->toString();
}

IncrementHighLevelOperation::IncrementHighLevelOperation(const std::string &name) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("increment operator can only be used on a variable");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> IncrementHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<IncrementPartialInstruction>(std::move(var)));
    return instructions;
}

std::string IncrementHighLevelOperation::toString() {
    return var->toString() +" ++";
}

DecrementHighLevelOperation::DecrementHighLevelOperation(const std::string &name) {
    var = parseDataType(name);
    if (!var->isVariable()) {
        throw std::runtime_error("increment operator can only be used on a variable");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> DecrementHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DecrementPartialInstruction>(std::move(var)));
    return instructions;
}

std::string DecrementHighLevelOperation::toString() {
    return var->toString() +" --";
}

UserFunctionHighLevelOperation::UserFunctionHighLevelOperation(std::string &functionName, const std::string &params, std::ifstream &file, int& lineNumber, std::vector<UserFunctionData>& functionData) : name(
    std::move(functionName)), returnsValue(false) {
    bool returnEndounmtered=false;
    //parsing args here
    std::string localParams = params;
    size_t camaPos = localParams.find(',');
    while (camaPos != std::string::npos) {
        std::string param = localParams.substr(0, camaPos);
        param = trim(param);
        paramaters.push_back(param);
        localParams = localParams.substr(camaPos + 1);
        camaPos = localParams.find(',');
    }
    localParams = trim(localParams);
    if (!localParams.empty()) {
        paramaters.push_back(localParams);
    }
    functionData.emplace_back(name, static_cast<int>(paramaters.size()));

    std::string functionLine;
    while (std::getline(file, functionLine)) {
        lineNumber++;
        std::string lineTrimmed = trim(functionLine);
        if (lineTrimmed[0] == '}') {
            //if the line starts with the end of the function then assume it is the end of the function
            return;
        }
        if (returnsValue) {//return must be the last statment in a funcion
            size_t commentStart = lineTrimmed.find("//");
            if (commentStart == std::string::npos) {
                commentStart = lineTrimmed.size();
            }
            //strip any comments to make sure this line is a blank
            std::string noComments = lineTrimmed.substr(0, commentStart);
            noComments = trim(noComments);
            if (!noComments.empty()) {
                throw std::runtime_error("Return must be the last statment of a funcion and can not be inside another block");
            }
        }
        std::unique_ptr<HighLevelConstruct> block = parseFileLine(lineTrimmed, file, lineNumber, localVars, !returnEndounmtered);
        if (lineTrimmed.starts_with("return")) {//if the line is a return statement
            returnEndounmtered = true;
            if (block != nullptr) {//if you return nothing then no contstucts will be genrated
                returnsValue = true;
                auto* rho = dynamic_cast<ReturnHighLevelOperation*>(block.get());
                returnValue = std::move(rho->value);
                continue;//do not add this to the list of internal blocks, return values can not be expanded they should be handled here
            }
        }
        if (block != nullptr) {
            //enure that a blank line was not just processed
            blocks.push_back(std::move(block));
        }
    }

    throw std::runtime_error("Reached end of file while processing function, expected function end!");
}

std::vector<std::unique_ptr<PartialInstruction>> UserFunctionHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    //need a unique type that can pass the function block info on to the next stage as well as expand each component

    //get the partial instructions for all the content of this function
    std::vector<std::unique_ptr<PartialInstruction>> partialInstructions;
    for (size_t i = 0; i < localVars.size(); i++) {//push space for local vars into the stack
        partialInstructions.emplace_back(std::make_unique<StackPushPartialInstruction>(std::make_unique<ZeroDataType>(),0));
    }
    //for each high level block
    for (auto &block : blocks) {
        //get the expanded content
        std::vector<std::unique_ptr<PartialInstruction>> tmp = block->expand();
        for (auto &instruction : tmp) {
            partialInstructions.push_back(std::move(instruction));//add that content to the overall instruction list
        }
    }
    if (returnsValue) {
        instructions.emplace_back(std::make_unique<BlockPartialInstruction>(std::move(partialInstructions),name,"",localVars,paramaters, std::move(returnValue)));
    } else {
        instructions.emplace_back(std::make_unique<BlockPartialInstruction>(std::move(partialInstructions),name,"",localVars,paramaters, nullptr));
    }
    return instructions;
}

std::string UserFunctionHighLevelOperation::toString() {
    std::stringstream ss;
    ss << "function"<<std::endl;
    for (auto &block: blocks) {
        ss <<"\t" << block->toString() << std::endl;
    }
    return ss.str();
}

CallUserFunctionHighLevelOperation::CallUserFunctionHighLevelOperation(std::string name, const std::string &rawParams): name(std::move(name)) {
    std::string localParams = rawParams;
    size_t camaPos = localParams.find(',');
    while (camaPos != std::string::npos) {
        std::string param = localParams.substr(0,camaPos);
        param = trim(param);
        params.push_back(parseDataType(param));
        localParams = localParams.substr(camaPos+1);
        camaPos = localParams.find(',');
    }
    localParams = trim(localParams);
    if (!localParams.empty()) {
        params.push_back(parseDataType(localParams));
    }
}

std::vector<std::unique_ptr<PartialInstruction>> CallUserFunctionHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    //push all params onto the stack
    //int the future remeber to account for return value!
    for (size_t i=0;i<params.size();i++) {
        //here i is also representing how much offset the stack currently has
        //the index af the actual param needs to be backwards tho
        size_t paramIndex = params.size()-i-1;
        instructions.emplace_back(std::make_unique<StackPushPartialInstruction>(std::move(params[paramIndex]),static_cast<int>(i)));
    }
    instructions.emplace_back(std::make_unique<FunctionCallPartialInstruction>(std::move(name),static_cast<int>(params.size())));
    for (int i = static_cast<int>(params.size())-1; i >= 0; i--) {
        //pop the params into rz as we no longer care about them
        instructions.emplace_back(std::make_unique<StackPopPartialInstruction>(std::make_unique<ZeroDataType>(),static_cast<int>(i)));
    }
    return instructions;
}

std::string CallUserFunctionHighLevelOperation::toString() {
    return "Call "+name+"()";
}

std::vector<std::unique_ptr<PartialInstruction>> TrapHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<TrapPartialInstruction>());
    return instructions;
}

std::string TrapHighLevelOperation::toString() {
    return "trap";
}

ReturnHighLevelOperation::ReturnHighLevelOperation(const std::string &value) {
    this->value = parseDataType(value);
}

std::vector<std::unique_ptr<PartialInstruction>> ReturnHighLevelOperation::expand() {
    throw std::runtime_error("Return statements can not be expanded. They must ba handled by the wrapping function");
}

std::string ReturnHighLevelOperation::toString() {
    return "return "+value->toString();
}
