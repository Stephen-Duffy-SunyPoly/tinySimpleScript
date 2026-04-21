#include "edisonHighLevel.hpp"

E_LCDClearFunction::E_LCDClearFunction(const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function clear. Too many parameters!");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_LCDClearFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("LCD_CLEAR_SCREEN",std::make_unique<ZeroDataType>()));
    return instructions;
}

std::string E_LCDClearFunction::toString() {
    return "clear";
}

E_LCDPrintFunction::E_LCDPrintFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type print. Empty parameter!");
    }
    //annoyingly this is going to be the one place where string constants are valid in this lang
    if (line[0] == '"') { //print a string literal
        std::string processLine = line.substr(1);
        size_t strEnd = processLine.find_last_of('"');
        std::string theRest = processLine.substr(strEnd+1);
        processLine = processLine.substr(0, strEnd);
        theRest = trim(theRest);
        if (!theRest.empty()) {//there should be nothing after the last "
            throw std::runtime_error("Invalid expression for function clear. "+line);
        }
        toPrint = processLine;
    } else {//print a regular memory value
        if (line.find(',') != std::string::npos) {
            throw std::runtime_error("Invalid parameters for function print. Too many parameters!");
        }
        std::string param1 = trim(line);
        if (param1.empty()) {
            throw std::runtime_error("Invalid parameters for function print. Missing param 1");
        }
        printValue = parseDataType(param1);
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_LCDPrintFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    if (toPrint.empty()) {//if just printing a value
        instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_CURSOR]",std::move(printValue)));
    } else {
        instructions.emplace_back(std::make_unique<PrintStringPartialInstruction>(toPrint,"LCD_CURSOR"));
    }
    return instructions;
}

std::string E_LCDPrintFunction::toString() {
    if (toPrint.empty()) {
        return "print "+printValue->toString();
    }
    return "print "+toPrint;
}
