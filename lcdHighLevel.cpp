#include "lcdHighLevel.hpp"

std::vector<PartialInstruction> UpdateFunction::expand() {
    //produced a single instruction that stores rZ in the UPDATE address
    //str [UPDATE], rZ
    return {};
}

//rectangle constructor
RectangleFunction::RectangleFunction(const std::string &line) {
    //get each parameter as a string
    size_t nextComa = line.find(',');
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type retangle. Not enough parameters!");
    }
    std::string param1 = line.substr(0, nextComa);
    std::string continueStr = line.substr(nextComa);
    nextComa = continueStr.find(',', nextComa);
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type retangle. Not enough parameters!");
    }
    std::string param2 = continueStr.substr(nextComa);
    continueStr = continueStr.substr(nextComa);
    nextComa = continueStr.find(',', nextComa);
    if (nextComa == std::string::npos) {
        throw std::runtime_error("Invalid parameters for function type retangle. Not enough parameters!");
    }
    std::string param3 = continueStr.substr(nextComa);
    continueStr = continueStr.substr(nextComa);
    nextComa = continueStr.find(',', nextComa);
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

std::vector<PartialInstruction> RectangleFunction::expand() {
    //for each param
    //load the param if they are not 0
    //store the value into appropriate address

    //store 0 into the rect address
    return {};
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

std::vector<PartialInstruction> FillFunction::expand() {
    return {};
}
