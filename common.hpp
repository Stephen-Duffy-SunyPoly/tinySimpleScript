#pragma once

#include <vector>

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

struct PartialInstruction {
    //not sure what is going here yet or if this will be a class
};

class HighLevelConstruct {
public:
    virtual ~HighLevelConstruct() = default;
    virtual std::vector<PartialInstruction> expand() = 0;
};
