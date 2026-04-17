#include "commonHighLevel.hpp"

#include <stdexcept>


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
