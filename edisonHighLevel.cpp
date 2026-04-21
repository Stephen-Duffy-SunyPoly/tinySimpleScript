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

E_ReadCursorFunction::E_ReadCursorFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read cursor. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadCursorFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_CURSOR"));
    return instructions;
}

std::string E_ReadCursorFunction::toString() {
    return "read cursor";
}

E_MoveCursorLeftFunction::E_MoveCursorLeftFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type move cursor left. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function move cursor left. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function move cursor left. Missing param 1");
    }
    amount = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_MoveCursorLeftFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_MOVE_LEFT]",std::move(amount)));
    return instructions;
}

std::string E_MoveCursorLeftFunction::toString() {
    return "Move cursor left "+amount->toString();
}

E_ReadLeftFunction::E_ReadLeftFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read left. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadLeftFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_MOVE_LEFT"));
    return instructions;
}

std::string E_ReadLeftFunction::toString() {
    return "read left";
}


E_MoveCursorRightFunction::E_MoveCursorRightFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type move cursor right. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function move cursor right. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function move cursor right. Missing param 1");
    }
    amount = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_MoveCursorRightFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_MOVE_RIGHT]",std::move(amount)));
    return instructions;
}

std::string E_MoveCursorRightFunction::toString() {
    return "Move cursor right "+amount->toString();
}

E_ReadRightFunction::E_ReadRightFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read right. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadRightFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_MOVE_RIGHT"));
    return instructions;
}

std::string E_ReadRightFunction::toString() {
    return "read right";
}

E_MoveCursorUpFunction::E_MoveCursorUpFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type move cursor up. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function move cursor up. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function move cursor up. Missing param 1");
    }
    amount = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_MoveCursorUpFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_MOVE_UP]",std::move(amount)));
    return instructions;
}

std::string E_MoveCursorUpFunction::toString() {
    return "Move cursor up "+amount->toString();
}

E_ReadUpFunction::E_ReadUpFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read up. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadUpFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_MOVE_UP"));
    return instructions;
}

std::string E_ReadUpFunction::toString() {
    return "read up";
}

E_MoveCursorDownFunction::E_MoveCursorDownFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type move cursor down. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function move cursor down. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function move cursor down. Missing param 1");
    }
    amount = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_MoveCursorDownFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_MOVE_DOWN]",std::move(amount)));
    return instructions;
}

std::string E_MoveCursorDownFunction::toString() {
    return "Move cursor down "+amount->toString();
}

E_ReadDownFunction::E_ReadDownFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read down. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadDownFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_MOVE_DOWN"));
    return instructions;
}

std::string E_ReadDownFunction::toString() {
    return "read down";
}

E_MoveCursorLeftWrapFunction::E_MoveCursorLeftWrapFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type move cursor left. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function move cursor left. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function move cursor left. Missing param 1");
    }
    amount = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_MoveCursorLeftWrapFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_MOVE_LEFT_WRAP]",std::move(amount)));
    return instructions;
}

std::string E_MoveCursorLeftWrapFunction::toString() {
    return "Move cursor left "+amount->toString();
}

E_ReadLeftWrapFunction::E_ReadLeftWrapFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read left. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadLeftWrapFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_MOVE_LEFT_WRAP"));
    return instructions;
}

std::string E_ReadLeftWrapFunction::toString() {
    return "read left";
}


E_MoveCursorRightWrapFunction::E_MoveCursorRightWrapFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type move cursor right. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function move cursor right. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function move cursor right. Missing param 1");
    }
    amount = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_MoveCursorRightWrapFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_MOVE_RIGHT_WRAP]",std::move(amount)));
    return instructions;
}

std::string E_MoveCursorRightWrapFunction::toString() {
    return "Move cursor right "+amount->toString();
}

E_ReadRightWrapFunction::E_ReadRightWrapFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read right. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadRightWrapFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_MOVE_RIGHT_WRAP"));
    return instructions;
}

std::string E_ReadRightWrapFunction::toString() {
    return "read right";
}

E_MoveCursorUpWrapFunction::E_MoveCursorUpWrapFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type move cursor up. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function move cursor up. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function move cursor up. Missing param 1");
    }
    amount = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_MoveCursorUpWrapFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_MOVE_UP_WRAP]",std::move(amount)));
    return instructions;
}

std::string E_MoveCursorUpWrapFunction::toString() {
    return "Move cursor up "+amount->toString();
}

E_ReadUpWrapFunction::E_ReadUpWrapFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read up. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadUpWrapFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_MOVE_UP_WRAP"));
    return instructions;
}

std::string E_ReadUpWrapFunction::toString() {
    return "read up";
}

E_MoveCursorDownWrapFunction::E_MoveCursorDownWrapFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type move cursor down. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function move cursor down. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function move cursor down. Missing param 1");
    }
    amount = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_MoveCursorDownWrapFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_MOVE_DOWN_WRAP]",std::move(amount)));
    return instructions;
}

std::string E_MoveCursorDownWrapFunction::toString() {
    return "Move cursor down "+amount->toString();
}

E_ReadDownWrapFunction::E_ReadDownWrapFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type read down. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadDownWrapFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_MOVE_DOWN_WRAP"));
    return instructions;
}

std::string E_ReadDownWrapFunction::toString() {
    return "read down";
}

E_SetCursorXFunction::E_SetCursorXFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type set cursor X. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function set cursor X. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function set cursor X. Missing param 1");
    }
    pos = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_SetCursorXFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_CURSOR_X]",std::move(pos)));
    return instructions;
}

std::string E_SetCursorXFunction::toString() {
    return "Set cursor X";
}

E_SetCursorYFunction::E_SetCursorYFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type set cursor Y. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function set cursor Y. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function set cursor Y. Missing param 1");
    }
    pos = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_SetCursorYFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_CURSOR_Y]",std::move(pos)));
    return instructions;
}

std::string E_SetCursorYFunction::toString() {
    return "Set cursor Y";
}

E_ReadCursorXFunction::E_ReadCursorXFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type get cursor Y. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadCursorXFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_CURSOR_X"));
    return instructions;
}

std::string E_ReadCursorXFunction::toString() {
    return "get cursor X";
}

E_ReadCursorYFunction::E_ReadCursorYFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type get cursor X. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_ReadCursorYFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_CURSOR_Y"));
    return instructions;
}

std::string E_ReadCursorYFunction::toString() {
    return "get cursor Y";
}

E_SetCursorPosFunction::E_SetCursorPosFunction(const std::string &line) {
    if (line.empty()) {
        throw std::runtime_error("Invalid parameters for function type set cursor pos. Empty parameter!");
    }
    if (line.find(',') != std::string::npos) {
        throw std::runtime_error("Invalid parameters for function set cursor pos. Too many parameters!");
    }
    std::string param1 = trim(line);
    if (param1.empty()) {
        throw std::runtime_error("Invalid parameters for function set cursor pos. Missing param 1");
    }
    pos = parseDataType(param1);
}

std::vector<std::unique_ptr<PartialInstruction>> E_SetCursorPosFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectStorPartialInstruction>("[LCD_CURSOR_XY]",std::move(pos)));
    return instructions;
}

std::string E_SetCursorPosFunction::toString() {
    return "Set cursor pos";
}

E_GetCursorPosFunction::E_GetCursorPosFunction(const std::string &retVar, const std::string &line) {
    if (!line.empty()) {
        throw std::runtime_error("Invalid parameters for function type get cursor position. Too many parameters");
    }
    returnValue = parseDataType(retVar);
    if (!returnValue->isVariable()) {
        throw std::runtime_error("Syntax error: functions can only return values to variables");
    }
}

std::vector<std::unique_ptr<PartialInstruction>> E_GetCursorPosFunction::expand() {
    std::vector<std::unique_ptr<PartialInstruction>> instructions;
    instructions.emplace_back(std::make_unique<DirectLoadPartialInstruction>(std::move(returnValue),"LCD_CURSOR_XY"));
    return instructions;
}

std::string E_GetCursorPosFunction::toString() {
    return "get cursor position";
}

