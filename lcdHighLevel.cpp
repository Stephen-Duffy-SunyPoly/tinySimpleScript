#include "lcdHighLevel.hpp"

#include <iostream>
#include <ostream>

std::vector<std::unique_ptr<PartialInstruction>> UpdateFunction::expand() {
    //produced a single instruction that stores rZ in the UPDATE address
    //str [UPDATE], rZ
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[UPDATE]", std::make_unique<ZeroDataType>()));
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
    x2Pos = parseDataType(param3);
    y2Pos = parseDataType(param4);
}

std::vector<std::unique_ptr<PartialInstruction>> RectangleFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    //for each param
    //load the param if they are not 0, actually the register cache system will handle loading and storing of variables
    //store the value into appropriate address
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[X1]",std::move(xPos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[Y1]",std::move(yPos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[X2]",std::move(x2Pos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[Y2]",std::move(y2Pos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[RECT]",std::make_unique<ZeroDataType>()));

    //store 0 into the rect address
    return instructions;
}

std::string RectangleFunction::toString() {
    return "rectangle ( "+xPos->toString()+", "+yPos->toString()+", "+x2Pos->toString()+", "+y2Pos->toString()+" )";
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
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[FILL]",std::move(color)));
    return instructions;
}

std::string FillFunction::toString() {
    return "fill ("+color->toString()+")";
}

StrokeFunction::StrokeFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type stroke. Empty parameter!");
    }

    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function stroke. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function stroke. Missing param 1");
    }
    color = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> StrokeFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[STROKE]",std::move(color)));
    return instructions;
}

std::string StrokeFunction::toString() {
    return "stroke ("+color->toString()+")";
}

SetFillFunction::SetFillFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type setFill. Empty parameter!");
    }

    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function setFill. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function setFill. Missing param 1");
    }
    value = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> SetFillFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[DRAWFILL]",std::move(value)));
    return instructions;
}

std::string SetFillFunction::toString() {
    return "setFill ("+value->toString()+")";
}

SetStrokeFunction::SetStrokeFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type setStroke. Empty parameter!");
    }

    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function setStroke. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function setStroke. Missing param 1");
    }
    value = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> SetStrokeFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[DRAWSTROKE]",std::move(value)));
    return instructions;
}

std::string SetStrokeFunction::toString() {
    return "setStroke ("+value->toString()+")";
}

PointFunction::PointFunction(const std::string &line) {
    //get each parameter as a string
    size_t nextComa = line.find(',');
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type point. Not enough parameters!");
    }
    std::string param1 = line.substr(0, nextComa);
    std::string continueStr = line.substr(nextComa+1);
    nextComa = continueStr.find(',');
    if (nextComa != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type point. Too many parameters!");
    }
    std::string param2 = continueStr;

    //trim each parameter
    param1 = trim(param1);
    param2 = trim(param2);
    //make sure there is actually somthing in all of them
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function type point. Missing param 1");
    }
    if (param2.empty()) {
        throw std::runtime_error("Invalid parameters for function type point. Missing param 2");
    }
    //parse them, figure out what they are
    xPos = parseDataType(param1);
    yPos = parseDataType(param2);
}

std::vector<std::unique_ptr<PartialInstruction>> PointFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[X1]",std::move(xPos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[Y1]",std::move(yPos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[POINT]",std::make_unique<ZeroDataType>()));
    return instructions;
}

std::string PointFunction::toString() {
    return "point ("+xPos->toString()+", "+yPos->toString()+")";
}

PrintFunction::PrintFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type print. Empty parameter!");
    }

    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function print. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function print. Missing param 1");
    }
    value = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> PrintFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[TERM]",std::move(value)));
    return instructions;
}

std::string PrintFunction::toString() {
    return "print ("+value->toString()+")";
}

std::vector<std::unique_ptr<PartialInstruction>> ClearFunction::expand() {

    //cant make tmp vars for these values, things do not work so well
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[X1]",std::make_unique<ZeroDataType>()));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[Y1]",std::make_unique<ZeroDataType>()));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[X2]",std::make_unique<ImmediateDataType>(63)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[Y2]",std::make_unique<ImmediateDataType>(63)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[RECT]",std::make_unique<ZeroDataType>()));
    return instructions;
}

std::string ClearFunction::toString() {
    return "clear";
}

LineFunction::LineFunction(const std::string &line) {
    //get each parameter as a string
    size_t nextComa = line.find(',');
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type line. Not enough parameters!");
    }
    std::string param1 = line.substr(0, nextComa);
    std::string continueStr = line.substr(nextComa+1);
    nextComa = continueStr.find(',');
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type line. Not enough parameters!");
    }
    std::string param2 = continueStr.substr(0,nextComa);
    continueStr = continueStr.substr(nextComa+1);
    nextComa = continueStr.find(',');
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type line. Not enough parameters!");
    }
    std::string param3 = continueStr.substr(0,nextComa);
    continueStr = continueStr.substr(nextComa+1);
    nextComa = continueStr.find(',');
    if (nextComa != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type line. Too many parameters!");
    }
    std::string param4 = continueStr;

    //trim each parameter
    param1 = trim(param1);
    param2 = trim(param2);
    param3 = trim(param3);
    param4 = trim(param4);
    //make sure there is actually somthing in all of them
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function type line. Missing param 1");
    }
    if (param2.empty()) {
        throw std::runtime_error("Invalid parameters for function type line. Missing param 2");
    }
    if (param3.empty()) {
        throw std::runtime_error("Invalid parameters for function type line. Missing param 3");
    }
    if (param4.empty()) {
        throw std::runtime_error("Invalid parameters for function type line. Missing param 4");
    }
    //parse them, figure out what they are
    xPos = parseDataType(param1);
    yPos = parseDataType(param2);
    x2Pos = parseDataType(param3);
    y2Pos = parseDataType(param4);
}

std::vector<std::unique_ptr<PartialInstruction>> LineFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    //for each param
    //load the param if they are not 0, actually the register cache system will handle loading and storing of variables
    //store the value into appropriate address
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[X1]",std::move(xPos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[Y1]",std::move(yPos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[X2]",std::move(x2Pos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[Y2]",std::move(y2Pos)));
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LINE]",std::make_unique<ZeroDataType>()));

    //store 0 into the rect address
    return instructions;
}

std::string LineFunction::toString() {
    return "line ( "+xPos->toString()+", "+yPos->toString()+", "+x2Pos->toString()+", "+y2Pos->toString()+" )";
}

ReadMouseXFunction::ReadMouseXFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read mouse X. Too many parameters");
    }
    retVal= parseDataType(retVar);
}

std::vector<std::unique_ptr<PartialInstruction>> ReadMouseXFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(retVal),"MOUSEX"));
    return instructions;
}

std::string ReadMouseXFunction::toString() {
    return "getMouseX";
}

ReadMouseYFunction::ReadMouseYFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read mouse Y. Too many parameters");
    }
    retVal= parseDataType(retVar);
}

std::vector<std::unique_ptr<PartialInstruction>> ReadMouseYFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(retVal),"MOUSEY"));
    return instructions;
}

std::string ReadMouseYFunction::toString() {
    return "getMouseX";
}

ReadMouseButtonFunction::ReadMouseButtonFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read port B. Too many parameters");
    }
    retVal= parseDataType(retVar);
}

std::vector<std::unique_ptr<PartialInstruction>> ReadMouseButtonFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(retVal),"MOUSEB"));
    return instructions;
}

std::string ReadMouseButtonFunction::toString() {
    return "getMouseButton";
}

ReadKeyboardFunction::ReadKeyboardFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read keyboard. Too many parameters");
    }
    retVal= parseDataType(retVar);
}

std::vector<std::unique_ptr<PartialInstruction>> ReadKeyboardFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(retVal),"KEY"));
    return instructions;
}

std::string ReadKeyboardFunction::toString() {
    return "getKeyboard";
}