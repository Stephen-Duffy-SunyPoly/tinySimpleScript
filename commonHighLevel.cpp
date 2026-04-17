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
        throw std::runtime_error("modulus operator left hand side must be a variable");
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
        throw std::runtime_error("modulus operator left hand side must be a variable");
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
        throw std::runtime_error("modulus operator left hand side must be a variable");
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
