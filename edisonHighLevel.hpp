#pragma once
#include "common.hpp"

class E_LCDClearFunction: public HighLevelConstruct {
public:
    E_LCDClearFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_LCDPrintFunction: public HighLevelConstruct {
    std::string toPrint;
    std::unique_ptr<DataType> printValue;
public:
    E_LCDPrintFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadCursorFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    E_ReadCursorFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_MoveCursorLeftFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> amount;
public:
    explicit E_MoveCursorLeftFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadLeftFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    E_ReadLeftFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_MoveCursorRightFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> amount;
public:
    explicit E_MoveCursorRightFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadRightFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    E_ReadRightFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_MoveCursorUpFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> amount;
public:
    explicit E_MoveCursorUpFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadUpFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    E_ReadUpFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_MoveCursorDownFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> amount;
public:
    explicit E_MoveCursorDownFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadDownFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    E_ReadDownFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_MoveCursorLeftWrapFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> amount;
public:
    explicit E_MoveCursorLeftWrapFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadLeftWrapFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    E_ReadLeftWrapFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_MoveCursorRightWrapFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> amount;
public:
    explicit E_MoveCursorRightWrapFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadRightWrapFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    E_ReadRightWrapFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_MoveCursorUpWrapFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> amount;
public:
    explicit E_MoveCursorUpWrapFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadUpWrapFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    E_ReadUpWrapFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_MoveCursorDownWrapFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> amount;
public:
    explicit E_MoveCursorDownWrapFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadDownWrapFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    E_ReadDownWrapFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_SetCursorXFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> pos;
public:
    explicit E_SetCursorXFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_SetCursorYFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> pos;
public:
    explicit E_SetCursorYFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadCursorXFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    explicit E_ReadCursorXFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_ReadCursorYFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    explicit E_ReadCursorYFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_SetCursorPosFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> pos;
public:
    explicit E_SetCursorPosFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_GetCursorPosFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    explicit E_GetCursorPosFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_SetBuzzerLeftFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> frequency;
public:
    explicit E_SetBuzzerLeftFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_SetBuzzerRightFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> frequency;
public:
    explicit E_SetBuzzerRightFunction(const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_GetFaderLeftFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    explicit E_GetFaderLeftFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class E_GetFaderRightFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> returnValue;
public:
    explicit E_GetFaderRightFunction(const std::string &retVar, const std::string &line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};