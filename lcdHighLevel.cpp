#include "lcdHighLevel.hpp"

#include <iostream>
#include <ostream>

std::vector<std::unique_ptr<PartialInstruction>> UpdateFunction::expand() {
    //produced a single instruction that stores rZ in the UPDATE address
    //str [UPDATE], rZ
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("UPDATE", std::make_unique<ZeroDataType>()));
    return instructions;
}

std::string UpdateFunction::toString() {
    return "update";
}

//rectangle constructor
RectangleFunction::RectangleFunction(const std::string &line) {
    //get each parameter as a string
    size_t nextComa = line.find(',');
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type retangle. Not enough parameters!");
    }
    std::string param1 = line.substr(0, nextComa);
    std::string continueStr = line.substr(nextComa+1);
    nextComa = continueStr.find(',');
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type retangle. Not enough parameters!");
    }
    std::string param2 = continueStr.substr(0,nextComa);
    continueStr = continueStr.substr(nextComa+1);
    nextComa = continueStr.find(',');
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type retangle. Not enough parameters!");
    }
    std::string param3 = continueStr.substr(0,nextComa);
    continueStr = continueStr.substr(nextComa+1);
    nextComa = continueStr.find(',');
    if (nextComa != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type rectangle. Too many parameters!");
    }
    std::string param4 = continueStr;

    //trim each parameter
    param1 = trim(param1);
    param2 = trim(param2);
    param3 = trim(param3);
    param4 = trim(param4);
    //make sure there is actually somthing in all of them
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function type rectangle. Missing param 1");
    }
    if (param2.empty()) {
        throw std::runtime_error("Invalid parameters for function type rectangle. Missing param 2");
    }
    if (param3.empty()) {
        throw std::runtime_error("Invalid parameters for function type rectangle. Missing param 3");
    }
    if (param4.empty()) {
        throw std::runtime_error("Invalid parameters for function type rectangle. Missing param 4");
    }
    //parse them, figure out what they are
    xPos = parseDataType(param1);
    yPos = parseDataType(param2);
    width = parseDataType(param3);
    height = parseDataType(param4);
}

std::vector<std::unique_ptr<PartialInstruction>> RectangleFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    //for each param
    //load the param if they are not 0, actually the register cache system will handle loading and storing of variables
    //store the value into appropriate address
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("X1",std::move(xPos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("Y1",std::move(yPos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("X2",std::move(width)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("Y2",std::move(height)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("RECT",std::make_unique<ZeroDataType>()));

    //store 0 into the rect address
    return instructions;
}

std::string RectangleFunction::toString() {
    return "rectangle ( "+xPos->toString()+", "+yPos->toString()+", "+width->toString()+", "+height->toString()+" )";
}

FillFunction::FillFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type fill. Empty parameter!");
    }

    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function fill. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function fill. Missing param 1");
    }
    color = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> FillFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("FILL",std::move(color)));
    return instructions;
}

std::string FillFunction::toString() {
    return "fill ("+color->toString()+")";
}
