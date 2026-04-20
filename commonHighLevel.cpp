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

    std::string functionLine;
    while (std::getline(file, functionLine)) {
        lineNumber++;
        std::string lineTrimmed = trim(functionLine);
        if (lineTrimmed[0] == '}') {
            //if the line starts with the end of the function then assume it is the end of the function
            //we won't know if we are returning something until we get to the end of the function so make this entry here
            functionData.emplace_back(name, static_cast<int>(paramaters.size()),returnsValue);
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
            if (block != nullptr) {//if you return nothing then no constructs will be generated
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
        partialInstructions.emplace_back(std::make_unique<StackPushPartialInstruction>(std::make_unique<ZeroDataType>(),0,"push space for "+localVars[localVars.size()-1-i]+" onto the stack"));
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
        instructions.emplace_back(std::make_unique<BlockPartialInstruction>(std::move(partialInstructions),name,"",localVars,paramaters, std::move(returnValue),false,nullptr,""));
    } else {
        instructions.emplace_back(std::make_unique<BlockPartialInstruction>(std::move(partialInstructions),name,"",localVars,paramaters, nullptr,false,nullptr,""));
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

CallUserFunctionHighLevelOperation::CallUserFunctionHighLevelOperation(std::string name, const std::string &rawParams, const std::string &returnTo): name(std::move(name)) {
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
    if (!returnTo.empty()) {
        returnValueTo = parseDataType(returnTo);//now we know its returning something!
    }
}

std::vector<std::unique_ptr<PartialInstruction>> CallUserFunctionHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    //push all params onto the stack

    instructions.emplace_back(std::make_unique<StackPushPartialInstruction>(std::make_unique<ZeroDataType>(),0,"push space for the return value onto the stack"));

    for (size_t i=0;i<params.size();i++) {
        //here i is also representing how much offset the stack currently has
        //the index af the actual param needs to be backwards tho
        size_t paramIndex = params.size()-i-1;
        instructions.emplace_back(std::make_unique<StackPushPartialInstruction>(std::move(params[paramIndex]),static_cast<int>(i+1),"push "+params[paramIndex]->toString()+" onto the stack as a parmater"));//add 1 for the return value regardless of if it gets used
    }

    instructions.emplace_back(std::make_unique<FunctionCallPartialInstruction>(std::move(name),static_cast<int>(params.size()),returnValueTo != nullptr));
    for (int i = static_cast<int>(params.size())-1; i >= 0; i--) {
        //pop the params into rz as we no longer care about them
        instructions.emplace_back(std::make_unique<StackPopPartialInstruction>(std::make_unique<ZeroDataType>(),i+1));
    }
    //pop the return value into the correct location is a return location was provided
    if (returnValueTo != nullptr) {
        instructions.emplace_back(std::make_unique<StackPopPartialInstruction>(std::move(returnValueTo),0));
    } else {
        instructions.emplace_back(std::make_unique<StackPopPartialInstruction>(std::make_unique<ZeroDataType>(),0));
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

int loopNameCounter = 1;

LoopHighLevelOperation::LoopHighLevelOperation(std::ifstream &file, int &lineNumber) {
    name = "LoopLabelInstruction_"+std::to_string(loopNameCounter++);
    //parsing args here
    std::string functionLine;
    while (std::getline(file, functionLine)) {
        lineNumber++;
        std::string lineTrimmed = trim(functionLine);
        if (lineTrimmed[0] == '}') {
            lineTrimmed = lineTrimmed.substr(1);
            lineTrimmed = trim(lineTrimmed);
            if (!lineTrimmed.starts_with("while")) {
                throw std::runtime_error("Syntax error. Expected 'while' after loop body");
            }
            lineTrimmed = lineTrimmed.substr(5);
            lineTrimmed = trim(lineTrimmed);
            if (lineTrimmed[0] != '(') {
                throw std::runtime_error("Syntax error. Expected '(' after while statement");
            }
            lineTrimmed = lineTrimmed.substr(1);
            lineTrimmed = trim(lineTrimmed);
            size_t index_of_close = lineTrimmed.find(')');
            if (index_of_close == std::string::npos) {
                throw std::runtime_error("Syntax error. Expected ')' after loop condition");
            }
            lineTrimmed = lineTrimmed.substr(0,index_of_close);
            lineTrimmed = trim(lineTrimmed);
            std::vector<std::string> tokens;
            size_t nextSpace = lineTrimmed.find_first_of(WHITESPACE);
            std::string trimmedCopy = lineTrimmed;
            if (nextSpace != std::string::npos) {
                do {
                    tokens.push_back(trimmedCopy.substr(0,nextSpace));
                    trimmedCopy = trimmedCopy.substr(nextSpace+1);
                    trimmedCopy = trim(trimmedCopy);
                } while ((nextSpace = trimmedCopy.find_first_of(WHITESPACE)) != std::string::npos);
            }
            if (!trimmedCopy.empty()) {
                tokens.push_back(trimmedCopy);//add the final token
            }

            if (tokens.size() != 3) {
                throw std::runtime_error("Syntax error. Invalid condition expression: "+lineTrimmed);
            }
            check1 = parseDataType(tokens[0]);
            check2 = parseDataType(tokens[2]);
            if (tokens[1] == "==") {
                condition = EQUALS;
            } else if (tokens[1] == "!=") {
                condition = NOT_EQUALS;
            } else if (tokens[1] == ">") {
                condition = GREATER_THAN;
            } else if (tokens[1] == "<") {
                condition = LESS_THAN;
            } else if (tokens[1] == "<=") {
                condition = LESS_THAN_OR_EQUALS;
            } else if (tokens[1] == ">=") {
                condition = GREATER_THAN_OR_EQUALS;
            } else {
                throw std::runtime_error("Syntax error. Invalid logical condition operator: "+tokens[1]);
            }
            return;
        }
        std::unique_ptr<HighLevelConstruct> block = parseFileLine(lineTrimmed, file, lineNumber, localVars, false);
        if (block != nullptr) {
            //enure that a blank line was not just processed
            blocks.push_back(std::move(block));
        }
    }

    throw std::runtime_error("Reached end of file while processing loop, expected loop end!");
}

std::vector<std::unique_ptr<PartialInstruction>> LoopHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    //need a unique type that can pass the function block info on to the next stage as well as expand each component

    //get the partial instructions for all the content of this function
    std::vector<std::unique_ptr<PartialInstruction>> partialInstructions;
    //this is handled in the bloc instruction now!
    // for (size_t i = 0; i < localVars.size(); i++) {//push space for local vars into the stack
    //     //this needs to be inside the loop but before the label
    //     instructions.emplace_back(std::make_unique<StackPushPartialInstruction>(std::make_unique<ZeroDataType>(),0, "push space for "+localVars[localVars.size()-1-i]+" onto the stack"));
    // }
    //for each high level block
    for (auto &block : blocks) {
        //get the expanded content
        std::vector<std::unique_ptr<PartialInstruction>> tmp = block->expand();
        for (auto &instruction : tmp) {
            partialInstructions.push_back(std::move(instruction));//add that content to the overall instruction list
        }
    }
    //special loop instructions
    std::vector<std::string> noParams;

    instructions.emplace_back(std::make_unique<FlushGlobalVarsPartialInstruction>());
    instructions.emplace_back(std::make_unique<BlockPartialInstruction>(std::move(partialInstructions),name+"_Start",name+"_Start",localVars,noParams, nullptr,true,
                                                                        std::make_unique<JumpConditionPartialInstruction>(std::move(check1),std::move(check2),condition,name+"_End"),name+"_End"));
    // instructions.emplace_back(std::make_unique<LabelPartialInstruction>(name+"_End"));
    return instructions;
}

std::string LoopHighLevelOperation::toString() {
    return "Loop";
}

IfHighLevelOperation::IfHighLevelOperation(const std::string& conditionRaw, std::ifstream &file, int &lineNumber){

    std::vector<std::string> tokens;
    size_t nextSpace = conditionRaw.find_first_of(WHITESPACE);
    std::string trimmedCopy = conditionRaw;
    if (nextSpace != std::string::npos) {
        do {
            tokens.push_back(trimmedCopy.substr(0,nextSpace));
            trimmedCopy = trimmedCopy.substr(nextSpace+1);
            trimmedCopy = trim(trimmedCopy);
        } while ((nextSpace = trimmedCopy.find_first_of(WHITESPACE)) != std::string::npos);
    }
    if (!trimmedCopy.empty()) {
        tokens.push_back(trimmedCopy);//add the final token
    }

    if (tokens.size() != 3) {
        throw std::runtime_error("Syntax error. Invalid condition expression: "+conditionRaw);
    }
    check1 = parseDataType(tokens[0]);
    check2 = parseDataType(tokens[2]);
    if (tokens[1] == "==") {
        condition = EQUALS;
    } else if (tokens[1] == "!=") {
        condition = NOT_EQUALS;
    } else if (tokens[1] == ">") {
        condition = GREATER_THAN;
    } else if (tokens[1] == "<") {
        condition = LESS_THAN;
    } else if (tokens[1] == "<=") {
        condition = LESS_THAN_OR_EQUALS;
    } else if (tokens[1] == ">=") {
        condition = GREATER_THAN_OR_EQUALS;
    } else {
        throw std::runtime_error("Syntax error. Invalid logical condition operator: "+tokens[1]);
    }

    //end of condition parsing
    bool elseBlock = false;
    std::string functionLine;
    while (std::getline(file, functionLine)) {
        lineNumber++;
        std::string lineTrimmed = trim(functionLine);
        //check for the end of the block
        if (lineTrimmed[0] == '}') {
            //check for else or and of the block
            lineTrimmed = lineTrimmed.substr(1);
            lineTrimmed = trim(lineTrimmed);
            if (lineTrimmed.empty()) {//we found the end
                return;
            }
            if (!elseBlock) {
                if (lineTrimmed.starts_with("else")) {
                    lineTrimmed = lineTrimmed.substr(4);
                    lineTrimmed = trim(lineTrimmed);
                    if (lineTrimmed.size() == 1) {
                        if (lineTrimmed[0] == '{') {
                            elseBlock = true;
                            continue;
                        }
                    }
                }
            }
            throw std::runtime_error("Syntax error. Invalid statement following } "+lineTrimmed);
        }

        if (elseBlock) {
            std::unique_ptr<HighLevelConstruct> block = parseFileLine(lineTrimmed, file, lineNumber, falseLocalVars, false);
            if (block != nullptr) {
                //enure that a blank line was not just processed
                falseBlocks.push_back(std::move(block));
            }
        } else {
            std::unique_ptr<HighLevelConstruct> block = parseFileLine(lineTrimmed, file, lineNumber, trueLocalVars, false);
            if (block != nullptr) {
                //enure that a blank line was not just processed
                trueBlocks.push_back(std::move(block));
            }
        }
    }
}

int ifStatementNumber =1;

std::vector<std::unique_ptr<PartialInstruction>> IfHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    std::vector<std::unique_ptr<PartialInstruction>> trueInstructions;
    std::vector<std::unique_ptr<PartialInstruction>> falseInstructions;
    std::vector<std::string> noParams;

    std::string trueLabel = "startIfStatmentLabel_"+std::to_string(ifStatementNumber);
    std::string endLabel = "endIfStatmentLabel_"+std::to_string(ifStatementNumber);
    std::string elseLabel;
    if (!falseBlocks.empty()) {
        elseLabel = "elseIfStatmentLabel_"+std::to_string(ifStatementNumber);
    } else {
        elseLabel = endLabel;
    }
    ifStatementNumber ++;
    //condition jump
    instructions.emplace_back(std::make_unique<FlushGlobalVarsPartialInstruction>());
    instructions.emplace_back(std::make_unique<JumpConditionPartialInstruction>(std::move(check1),std::move(check2),condition,elseLabel));

    //<inside true block> push local vars
    for (size_t i=0;i<trueLocalVars.size();i++) {
        trueInstructions.emplace_back(std::make_unique<StackPushPartialInstruction>(std::make_unique<ZeroDataType>(),0, "push space for "+trueLocalVars[trueLocalVars.size()-1-i]+" onto the stack"));
    }
    for (auto &thing: trueBlocks) {
        std::vector<std::unique_ptr<PartialInstruction>> tmp = thing->expand();
        for (auto &instruction: tmp) {
            trueInstructions.push_back(std::move(instruction));
        }
    }
    instructions.emplace_back(std::make_unique<BlockPartialInstruction>(std::move(trueInstructions),trueLabel,endLabel, trueLocalVars,noParams,nullptr,false,nullptr,""));
    //expand true instruction into true block

    if (!falseBlocks.empty()) {
        //<optional> false block
        for (size_t i=0;i<falseLocalVars.size();i++) {
            falseInstructions.emplace_back(std::make_unique<StackPushPartialInstruction>(std::make_unique<ZeroDataType>(),0,"push space for "+falseLocalVars[falseLocalVars.size()-1-i]+" onto the stack"));
        }

        //<inside false block> push false local vars
        //expand false instructions
        for (auto &thing: falseBlocks) {
            std::vector<std::unique_ptr<PartialInstruction>> tmp = thing->expand();
            for (auto &instruction: tmp) {
                falseInstructions.push_back(std::move(instruction));
            }
        }
        instructions.emplace_back(std::make_unique<BlockPartialInstruction>(std::move(falseInstructions),elseLabel,endLabel, falseLocalVars,noParams,nullptr,false,nullptr,""));
    }
    //end label
    instructions.emplace_back(std::make_unique<LabelPartialInstruction>(endLabel));

    return instructions;
}

std::string IfHighLevelOperation::toString() {
    return "If";
}

DelayFunction::DelayFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type delay. Empty parameter!");
    }

    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function delay. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function delay. Missing param 1");
    }
    amount = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> DelayFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DelayPartialInstruction>(std::move(amount)));
    return instructions;
}

std::string DelayFunction::toString() {
    return "delay "+amount->toString();
}

RandFunction::RandFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type random. Too many parameters");
    }
    returnTo = parseDataType(retVar);
}

std::vector<std::unique_ptr<PartialInstruction>> RandFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnTo),"RAND"));
    return instructions;
}

std::string RandFunction::toString() {
    return "Random";
}

RandBitsFunction::RandBitsFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type randomBits. Too many parameters");
    }
    returnTo = parseDataType(retVar);
}

std::vector<std::unique_ptr<PartialInstruction>> RandBitsFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnTo),"RAND_BITS"));
    return instructions;
}

std::string RandBitsFunction::toString() {
    return "Bits";
}

ReadPortAFunction::ReadPortAFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read port A. Too many parameters");
    }
    returnTo = parseDataType(retVar);
}

std::vector<std::unique_ptr<PartialInstruction>> ReadPortAFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnTo),"PORT_A"));
    return instructions;
}

std::string ReadPortAFunction::toString() {
    return "ReadPortA";
}

ReadPortBFunction::ReadPortBFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read port B. Too many parameters");
    }
    returnTo = parseDataType(retVar);
}

std::vector<std::unique_ptr<PartialInstruction>> ReadPortBFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnTo),"PORT_B"));
    return instructions;
}

std::string ReadPortBFunction::toString() {
    return "ReadPortB";
}

SetPortAFunction::SetPortAFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type setPortA. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function setPortA. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function setPortA. Missing param 1");
    }
    setTo = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> SetPortAFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("PORT_A",std::move(setTo)));
    return instructions;
}

std::string SetPortAFunction::toString() {
    return "SetPortA";
}

SetPortBFunction::SetPortBFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type setPortB. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function setPortB. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function setPortB. Missing param 1");
    }
    setTo = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> SetPortBFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("PORT_B",std::move(setTo)));
    return instructions;
}

std::string SetPortBFunction::toString() {
    return "SetPortB";
}

SetPortADirectionFunction::SetPortADirectionFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type setPortADirection. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function setPortADirection. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function setPortADirection. Missing param 1");
    }
    setTo = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> SetPortADirectionFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("PORT_A_DIR",std::move(setTo)));
    return instructions;
}

std::string SetPortADirectionFunction::toString() {
    return "SetPortADirection";
}

SetPortBDirectionFunction::SetPortBDirectionFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type setPortBDirection. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function setPortBDirection. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function setPortBDirection. Missing param 1");
    }
    setTo = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> SetPortBDirectionFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("PORT_B_DIR",std::move(setTo)));
    return instructions;
}

std::string SetPortBDirectionFunction::toString() {
    return "SetPortBDirection";
}

RightShiftHighLevelOperation::RightShiftHighLevelOperation(const std::string &var, const std::string &by) {
    data = parseDataType(var);
    if (!data->isVariable()) {
        throw std::runtime_error("right shift opprator left hand side must be a variable ");
    }
    amount = parseDataType(by);
}

std::vector<std::unique_ptr<PartialInstruction>> RightShiftHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<RightShiftPartialInstruction>(std::move(data),std::move(amount)));
    return instructions;
}

std::string RightShiftHighLevelOperation::toString() {
    return "right shift "+data->toString() +" by "+amount->toString();
}

LeftShiftHighLevelOperation::LeftShiftHighLevelOperation(const std::string &var, const std::string &by) {
    data = parseDataType(var);
    if (!data->isVariable()) {
        throw std::runtime_error("left shift operator left hand side must be a variable ");
    }
    amount = parseDataType(by);
}

std::vector<std::unique_ptr<PartialInstruction>> LeftShiftHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<LeftShiftPartialInstruction>(std::move(data),std::move(amount)));
    return instructions;
}

std::string LeftShiftHighLevelOperation::toString() {
    return "left shift "+data->toString() +" by "+amount->toString();
}

NegateHighLevelOperation::NegateHighLevelOperation(const std::string &var) {
    data = parseDataType(var);
    if (!data->isVariable()) {
        throw std::runtime_error("left shift operator left hand side must be a variable ");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> NegateHighLevelOperation::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<NegatePartialInstruction>(std::move(data)));
    return instructions;
}

std::string NegateHighLevelOperation::toString() {
    return "negate "+data->toString();
}
