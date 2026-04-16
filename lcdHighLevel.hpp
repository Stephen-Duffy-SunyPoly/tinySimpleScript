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
    std::vector<PartialInstruction> expand() override;
};

class RectangleFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> xPos;
    std::unique_ptr<DataType> yPos;
    std::unique_ptr<DataType> width;
    std::unique_ptr<DataType> height;
    public:
    /**
     * @param line The data trimmed between the parentheses
     */
    explicit RectangleFunction(const std::string& line);
    std::vector<PartialInstruction> expand() override;
};

class FillFunction: public HighLevelConstruct {
    std::unique_ptr<DataType> color;
    public:
    explicit FillFunction([[maybe_unused]] const std::string& line);
    std::vector<PartialInstruction> expand() override;
};