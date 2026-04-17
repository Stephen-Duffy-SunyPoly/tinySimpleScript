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
