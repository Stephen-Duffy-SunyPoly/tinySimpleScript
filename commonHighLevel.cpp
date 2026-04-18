#include "commonHighLevel.hpp"

#include <fstream>
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

UserFunctionHighLevelOperation::UserFunctionHighLevelOperation(std::string name, const std::string &params, std::ifstream &file, int& lineNumber) : name(std::move(name)){
//parsing args here
    std::string functionLine;
    while (std::getline(file, functionLine)) {
        lineNumber++;
        std::string lineTrimmed = trim(functionLine);
        if (lineTrimmed[0] == '}') {//if the line starts with the end of the function then assume it is the end of the function
            return;
        }
        std::unique_ptr<HighLevelConstruct> block = parseFileLine(lineTrimmed,file,lineNumber, localVars);
        if (block != nullptr) {//enure that a blank line was not just processed
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
        partialInstructions.emplace_back(std::make_unique<StackPushPartialInstruction>(std::make_unique<ZeroDataType>()));
    }
    //for each high level block
    for (auto &block : blocks) {
        //get the expanded content
        std::vector<std::unique_ptr<PartialInstruction>> tmp = block->expand();
        for (auto &instruction : tmp) {
            partialInstructions.push_back(std::move(instruction));//add that content to the overall instrution list
        }
    }
    instructions.emplace_back(std::make_unique<BlockPartialInstruction>(std::move(partialInstructions),name,"",localVars));
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

std::vector<std::unique_ptr<PartialInstruction>> CallUserFunctionHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    //TODO params
    instructions.emplace_back(std::make_unique<FunctionCallPartialInstruction>(std::move(name)));
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
