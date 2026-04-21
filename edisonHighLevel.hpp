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