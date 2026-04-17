#pragma once
#include <memory>
#include <stdexcept>
#include <string>

#include "common.hpp"

class UpdateFunction: public HighLevelConstruct {
    public:
    /**
     * @param line The data trimmed between the parentheses
     */
    explicit UpdateFunction([[maybe_unused]] const std::string& line) {
        //for update, we do not need any parameters
        if (!line.empty()) {
            throw std::runtime_error("Invalid parameters for function type update");
        }
    }
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class RectangleFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> xPos;
    std::unique_ptr<DataType> yPos;
    std::unique_ptr<DataType> x2Pos;
    std::unique_ptr<DataType> y2Pos;
    public:
    /**
     * @param line The data trimmed between the parentheses
     */
    explicit RectangleFunction(const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class FillFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> color;
public:
    explicit FillFunction([[maybe_unused]] const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class StrokeFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> color;
public:
    explicit StrokeFunction([[maybe_unused]] const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class SetFillFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> value;
public:
    explicit SetFillFunction([[maybe_unused]] const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class SetStrokeFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> value;
public:
    explicit SetStrokeFunction([[maybe_unused]] const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class PointFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> xPos;
    std::unique_ptr<DataType> yPos;
public:
    explicit PointFunction([[maybe_unused]] const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class PrintFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> value;
public:
    explicit PrintFunction([[maybe_unused]] const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class ClearFunction: public HighLevelConstruct {
public:
    explicit ClearFunction([[maybe_unused]] const std::string& line){
        //for clear, we do not need any parameters
        if (!line.empty()) {
            throw std::runtime_error("Invalid parameters for function type clear");
        }
    }
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};

class LineFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> xPos;
    std::unique_ptr<DataType> yPos;
    std::unique_ptr<DataType> x2Pos;
    std::unique_ptr<DataType> y2Pos;
public:
    /**
     * @param line The data trimmed between the parentheses
     */
    explicit LineFunction(const std::string& line);
    std::vector<std::unique_ptr<PartialInstruction>> expand() override;
    std::string toString() override;
};