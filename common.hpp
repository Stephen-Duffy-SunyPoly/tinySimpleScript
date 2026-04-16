#pragma once

#include <string>
#include <utility>
#include <vector>

const std::string WHITESPACE = " \t\n\r\f\v";

//why does c++ not have one of these?
/**Trims a string, removing all leading and trailing whitespace
 * @param str the string to trim
 * @return A string with no leading or trailing whitespace
 */
inline std::string trim(const std::string& str) {
    const auto first = str.find_first_not_of(WHITESPACE);
    if (first == std::string::npos) return "";
    const auto last = str.find_last_not_of(WHITESPACE);
    return str.substr(first, (last - first + 1));
}

inline bool charIsNumber(char c) {
    return c >= '0' && c <= '9';
}

inline bool stringIsNumber(const std::string &str) {
    return str.find_first_not_of("0123456789") == std::string::npos;
}

struct Register {
    int regNumber; //the actual register in use
    int lru = 0; // leased recently used value
    bool dirty = false;//if this register has been written too since it was loaded / last saved (if so it will have to be saved to memory before overriding
    bool immediate = false;//if the current value of this register was used for an immediate instead of a variable
    bool immediateUsed = false;//if this register currently represents and immediate and has been used. if it has not been used then do not override. if it has been used then feel free to override
    //TODO linked var

    explicit Register(int regNumber) : regNumber(regNumber) {}
    void operator ++(int) {
        lru++;
    }
};

class DataType {
public:
    virtual ~DataType() =default;
    virtual std::string toString() = 0;
};

class VariableDataType : public DataType {
    bool stackVar = false;
    bool resolved = false;
    int address = 0;//address if global, stack offset if local
    std::string varName;
public:
    explicit VariableDataType(std::string varName) : varName(std::move(varName)) {}
    std::string toString() override {
        return "var_"+varName;
    }
};

class ImmediateDataType : public DataType {
    int value = 0;
public:
    explicit ImmediateDataType(int value) : value(value) {}
    [[nodiscard]] int getValue() const {
        return value;
    }
    std::string toString() override {
        return std::to_string(value);
    }
};

class ZeroDataType : public DataType {
    public:
    std::string toString() override {
        return "rz";
    }
};


class PartialInstruction {
public:
    virtual ~PartialInstruction() =default;
    virtual std::string toString() = 0;
};

class DirectStorPartialInstruction : public PartialInstruction {
    std::string storeTo;
    std::unique_ptr<DataType> value;
public:
    explicit DirectStorPartialInstruction(std::string storeTo, std::unique_ptr<DataType> value) : storeTo(std::move(storeTo)), value(std::move(value)) {}
    std::string toString() override {
        return "store "+value->toString()+" in "+storeTo;
    }
};

class HighLevelConstruct {
public:
    virtual ~HighLevelConstruct() = default;
    virtual std::vector<std::unique_ptr<PartialInstruction>> expand() = 0;
    virtual std::string toString() = 0;
};


/**parse a parameter into a datatype
 * @param value The trimmed string passed into something as a parameter
 * @return a unique pointer to a representation of the datatype
 */
inline std::unique_ptr<DataType> parseDataType(const std::string& value) {
    //get 0 out of the way first
    if (value.size() == 1 && value[0] == '0') {
        return std::make_unique<ZeroDataType>();
    }
    if (charIsNumber(value[0])) {//check for immediate
        if (!stringIsNumber(value)) {
            throw std::runtime_error("Error parsing data type: variables can not start with numbers");
        }
        int numberValue = std::stoi(value);
        return std::make_unique<ImmediateDataType>(numberValue);
    }
    //its a variable, make sure there is no whitespace in the name
    if (value.find_first_of(WHITESPACE) != std::string::npos) {
        throw std::runtime_error("Error parsing data type: variables can not contain with whitespace");
    }
    return std::make_unique<VariableDataType>(value);
}