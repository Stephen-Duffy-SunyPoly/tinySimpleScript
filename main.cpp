#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>
#include <unordered_map>
#include <functional>
#include "common.hpp"
#include "commonHighLevel.hpp"
#include "lcdHighLevel.hpp"
#include <cpp-subprocess/subprocess.hpp>

using namespace std;

string systemConsts = "; TeenyAT Constants\n"
".const PORT_A_DIR   0x8000\n"
".const PORT_B_DIR   0x8001\n"
".const PORT_A       0x8002\n"
".const PORT_B       0x8003\n"
".const RAND         0x8010\n"
".const RAND_BITS    0x8011\n";

string lcdConsts = "; LCD Peripherals\n"
".const LIVESCREEN 0x9000\n"
".const UPDATESCREEN 0xA000\n"
".const X1 0xD000\n"
".const Y1 0xD001\n"
".const X2 0xD002\n"
".const Y2 0xD003\n"
".const STROKE 0xD010\n"
".const FILL 0xD011\n"
".const DRAWFILL 0xD012\n"
".const DRAWSTROKE 0xD013\n"
".const UPDATE 0xE000\n"
".const RECT 0xE010\n"
".const LINE 0xE011\n"
".const POINT 0xE012\n"
".const MOUSEX 0xFFFC\n"
".const MOUSEY 0xFFFD\n"
".const MOUSEB 0xFFFB\n"
".const TERM 0xFFFF\n"
".const KEY 0xFFFE";

const vector<string> reservedWords = {//note to self, add other opperators to this list
    "PORT_A_DIR", "PORT_B_DIR", "PORT_A", "PORT_B","RAND","RAND_BITS","LIVESCREEN","UPDATESCREEN","X1","Y1","X2","Y2","STROKE","FILL","DRAWFILL","DRAWSTROKE",
    "UPDATE", "RECT", "LINE", "POINT", "MOUSEX", "MOUSEY", "MOUSEB", "TERM", "KEY", "gvar", "lvar", "=", "function"
};

//the default mode of this lang is for the LCD system, the edison system, will be usable by a flag

//this process pipeline include several stages:
/* 1. Expansion - expand macro function into their required assembly, this includes thing like rect or variable assignment. this stage includes no hard registers
 * 2. variable assignment - all values except immediate get represented by a variable who gets an assigned memory offset or stack offset
 * 3. cached register assignment - all variables and required immediate are assigned to registers for usage in a style where registers are treated as a fully associated cache
*/
//the variable assignment stage is where the names of variables existing will be resolved

//each arrow here represents one of the stages above
// hi level built in functions and constructs -> partial instructions -> unfinished assembly -> finished assembly

//this lang will feature 2 types of variables, global heap vars and local stack vars
//there will also be user defined constants in a global scope


bool LCDSystem = true;

//description of the high level block for parsing
struct HighLevelDescription {
    function<unique_ptr<HighLevelConstruct>(const string &line)> create;
};

//the built in functions
unordered_map<string, HighLevelDescription> expansionFunctions;

vector<string> globalVars;
vector<string> functions;

vector<Register> registers;

unique_ptr<HighLevelConstruct> parseFileLine(const string& line, ifstream& file, int &lineNumber) {
    size_t commentStart = line.find("//");
    if (commentStart == string::npos) {
        commentStart = line.size();
    }
    string noComments = line.substr(0, commentStart);
    string lineTrimmed = trim(noComments);
    if (lineTrimmed.empty()) {
        return nullptr;
    }
    //determine if it is prbly a function call or not a function call.
    size_t parenthesisIndex = lineTrimmed.find('(');
    cout << parenthesisIndex <<" "<<lineTrimmed << endl;
    if (parenthesisIndex != string::npos) {
        //check for function def
        size_t firstSpace = lineTrimmed.find_first_of(WHITESPACE);
        string firstToken = lineTrimmed.substr(0,firstSpace);
        firstToken = trim(firstToken);
        if (firstToken == "function") {//its a funcitond def!
            lineTrimmed = lineTrimmed.substr(firstSpace+1);//remove the word function from the start of the line
            parenthesisIndex = lineTrimmed.find('(');
            string functionName = lineTrimmed.substr(0,parenthesisIndex);
            functionName = trim(functionName);
            //validate the name
            if (charIsNumber(functionName[0])) {
                throw std::runtime_error("Variable can not start with numbers");
            }
            //check if it is a allready reserved word
            for (const string &word:reservedWords) {
                if (word == functionName) {
                    throw std::runtime_error("Variable name " + functionName + " not allowed");
                }
            }
            //check if it is the same name as a pre defined function
            if (expansionFunctions.contains(functionName)) {
                throw std::runtime_error("Function name " + functionName + " not allowed");
            }
            //check if it is already defined
            for (const auto& s: globalVars) {
                if (s == functionName) {
                    throw std::runtime_error("Function name " + functionName + " already defined");
                }
            }
            //check if a function with this name already exists
            for (const auto& s: functions) {
                if (s == functionName) {
                    throw std::runtime_error("Function name " + functionName + " already defined");
                }
            }
            //get the parameters
            string params = lineTrimmed.substr(parenthesisIndex+1);
            size_t closeParenthesisIndex = params.find(')');
            if (closeParenthesisIndex == string::npos) {
                throw std::runtime_error("Syntax Error, unclosed function call, expected ')'");
            }
            size_t openBraceIndex = params.find('{');
            if (openBraceIndex == string::npos || openBraceIndex < closeParenthesisIndex) {
                throw std::runtime_error("Syntax Error, Function opening expected '{' after ')");
            }
            functions.push_back(functionName);
            return make_unique<UserFunctionHighLevelOperation>(functionName,params,file,lineNumber);
        } else {
            //its a function!!!!
            string functionName = lineTrimmed.substr(0,parenthesisIndex);
            functionName = trim(functionName);
            //get the parameters
            string params = lineTrimmed.substr(parenthesisIndex+1);
            size_t closeParenthesisIndex = params.find(')');
            if (closeParenthesisIndex == string::npos) {
                throw std::runtime_error("Syntax Error, unclosed function call, expected ')'");
            }
            params = params.substr(0,closeParenthesisIndex);
            params = trim(params);
            if (expansionFunctions.contains(functionName)) {
                return expansionFunctions[functionName].create(params);
            } else {
                //it might be a user defined function!
                //do that here.
                //for now tho:
                throw std::runtime_error("Function " + functionName + " not found");
            }
        }
    } else {
        //another type of expression
        vector<string> tokens;
        //tokenize the line
        size_t firstSpace = lineTrimmed.find_first_of(WHITESPACE);
        do {
            tokens.push_back(lineTrimmed.substr(0,firstSpace));
            lineTrimmed = lineTrimmed.substr(firstSpace+1);
            lineTrimmed = trim(lineTrimmed);
        } while ((firstSpace = lineTrimmed.find_first_of(WHITESPACE)) != string::npos);
        if (!lineTrimmed.empty()) {
            tokens.push_back(lineTrimmed);//add the final token
        }
        //first check the things who have a significant token as the first token
        if (tokens[0] == "gvar") { //declaring global vars
            //token 1 will be the name of the var
            if (tokens.size() > 1) {
                if (charIsNumber(tokens[1][0])) {
                    throw std::runtime_error("Variable can not start with numbers");
                }
                //check if it is a allready reserved word
                for (const string &word:reservedWords) {
                    if (word == tokens[1]) {
                        throw std::runtime_error("Variable name " + tokens[1] + " not allowed");
                    }
                }
                //check if it is the same name as a pre defined function
                if (expansionFunctions.contains(tokens[1])) {
                    throw std::runtime_error("Variable name " + tokens[1] + " not allowed");
                }
                //check if it is already defined
                for (const auto& s: globalVars) {
                    if (s == tokens[1]) {
                        throw std::runtime_error("Variable name " + tokens[1] + " already defined");
                    }
                }
                //check if it is defined as a funcion
                for (const auto& s: functions) {
                    if (s == tokens[1]) {
                        throw std::runtime_error("Variable name " + tokens[1] + " already defined");
                    }
                }
                //great not lets define it
                globalVars.push_back(tokens[1]);
                //check if there is more to this
                if (tokens.size() > 2) {
                    //add an assignment instruction
                    if (tokens[2] != "=") {
                        throw std::runtime_error("Syntax error. Expected = or nothing but got "+tokens[2]);
                    }
                    if (tokens.size() > 3) {
                        return make_unique<VariableAssignment>(tokens[1], tokens[3]);
                    } else {
                        throw std::runtime_error("Syntax error. Expected assignment right hand side but got nothing");
                    }
                }
            } else {
                throw std::runtime_error("Syntax error. Expected identifier after gvar but got nothing");
            }
            return nullptr;
        }
        //end of first token significance
        if (tokens.size() == 1) {
            throw std::runtime_error("Syntax error. Invalid statement: "+ noComments);
        }
        //there are at leased 2 tokens now
        //this is where ++ and -- go
        if (tokens[1] == "++") {
            return make_unique<IncrementHighLevelOperation>(tokens[0]);
        } else if (tokens[1] == "--") {
            return make_unique<DecrementHighLevelOperation>(tokens[0]);
        }

        if (tokens.size() < 3) {
            throw std::runtime_error("Syntax error. Invalid statement: "+ noComments);
        }
        //there are at lasted 3 tokens now
        //var opperators
        if (tokens[1] == "+=") {
            return make_unique<AddHighLevelOperation>(tokens[0], tokens[2]);
        } else if (tokens[1] == "-=") {
            return make_unique<SubtractHighLevelOperation>(tokens[0], tokens[2]);
        } else if (tokens[1] == "*=") {
            return make_unique<MultiplyHighLevelOperation>(tokens[0], tokens[2]);
        } else if (tokens[1] == "/=") {
            return make_unique<DivideHighLevelOperation>(tokens[0], tokens[2]);
        } else if (tokens[1] == "%=") {
            return make_unique<ModulusHighLevelOperation>(tokens[0], tokens[2]);
        } else if (tokens[1] == "&=") {
            return make_unique<AndHighLevelOperation>(tokens[0], tokens[2]);
        } else if (tokens[1] == "|=") {
            return make_unique<OrHighLevelOperation>(tokens[0], tokens[2]);
        } else if (tokens[1] == "^=") {
            return make_unique<XorHighLevelOperation>(tokens[0], tokens[2]);
        }

    }
    return nullptr;
}

int main(const int argc, char* argv[]) {
    //creat the registers
    for (int i=0;i<5;i++) {
        registers.emplace_back(i);
    }
    vector<string> args(argv, argv + argc);
    //remove first arg from vector
    args.erase(args.begin());

    //check for input file
    if (args.empty()) {
        cerr << "Usage: " << argv[0] << " <.tass file> [options]" << endl;
        return EXIT_FAILURE;
    }

    bool assemble = false;
    string assembler;

    //parse arguments
    for (size_t i=1;i<args.size();i++) {
        if (args[i] == "--tnasm") {
            if (args.size() > i+1) {
                assembler = args[++i];
                assemble = true;
                i++;
            } else {
                cerr << "Expected file path to tnasm but got nothing"<<endl;
                return EXIT_FAILURE;
            }
        }
    }

    ifstream fileIn(args[0]);
    if (!fileIn.is_open()) {
        cerr << "Failed to open file " << filesystem::absolute(args[0]) << endl;
        return EXIT_FAILURE;
    }

    //load the langue constructs:
    if (LCDSystem) {
        expansionFunctions.insert({"update",{[](const string &line) {return make_unique<UpdateFunction>(line);}}});
        expansionFunctions.insert({"rect",{[](const string &line) {return make_unique<RectangleFunction>(line);}}});
        expansionFunctions.insert({"fill",{[](const string &line) {return make_unique<FillFunction>(line);}}});
        expansionFunctions.insert({"stroke",{[](const string &line) {return make_unique<StrokeFunction>(line);}}});
        expansionFunctions.insert({"setFill",{[](const string &line) {return make_unique<SetFillFunction>(line);}}});
        expansionFunctions.insert({"setStroke",{[](const string &line) {return make_unique<SetStrokeFunction>(line);}}});
        expansionFunctions.insert({"point",{[](const string &line) {return make_unique<PointFunction>(line);}}});
        expansionFunctions.insert({"print",{[](const string &line) {return make_unique<PrintFunction>(line);}}});
        expansionFunctions.insert({"clear",{[](const string &line) {return make_unique<ClearFunction>(line);}}});
        expansionFunctions.insert({"line",{[](const string &line) {return make_unique<LineFunction>(line);}}});
    } else {
        //load edison system specific
    }

    vector<unique_ptr<HighLevelConstruct>> highLevelBlocks;

    string line;
    int lineNumber = 0;
    //parse each line into the high level implementation
    while (getline(fileIn, line)) {
        lineNumber++;
        try {
            unique_ptr<HighLevelConstruct> block = parseFileLine(line,fileIn,lineNumber);
            if (block != nullptr) {//enure that a blank line was not just processed
                highLevelBlocks.push_back(std::move(block));
            }
        } catch (exception& e) {
            cerr << "Error while processing line "<< lineNumber <<": "<<e.what() << endl;
            return EXIT_FAILURE;
        }
    }

    //Expansion
    vector<unique_ptr<PartialInstruction>> partialInstructions;
    //for each high level block
    for (auto &block : highLevelBlocks) {
        //get the expanded content
        vector<unique_ptr<PartialInstruction>> tmp = block->expand();
        for (auto &instruction : tmp) {
            partialInstructions.push_back(std::move(instruction));//add that content to the overall instrution list
        }
    }

    //variable assignment
    //TODO
    //loop through everything and pull out the variable declarations  and make them proper memory addresses

    //check all instructions to make sure all of their variables actually exist / resolve them
    try {
        for (auto &instruction : partialInstructions) {
            for (int i=0;i<instruction->numVars();i++) {
                unique_ptr<DataType> &dataInput = instruction->getVariable(i);
                if (dataInput != nullptr && dataInput->isVariable()) {
                    VariableDataType &var = dynamic_cast<VariableDataType&>(*dataInput);
                    //check if it is a global var:
                    bool found = false;
                    for (size_t j=0;j<globalVars.size();j++) {
                        if (globalVars[j] == var.getVarName()) {//found it!
                            var.resolve(false,static_cast<int>(j));
                            found = true;
                            break;
                        }
                    }
                    if (found) {
                        continue;
                    }
                    //check stack vars
                    //TODO
                    //didnt find it
                    throw std::runtime_error("Variable " + var.getVarName() + " not found");
                }
            }
        }
    } catch (exception& e) {
        cerr << "Error while resolving variables: "<<e.what() << endl;
        return EXIT_FAILURE;
    }

    //cached register assignment

    //IMPORTANT before function calls, all registers that contain global vars that are dirty must flush their values to ram. This also goes for the end of any function

    //each instruction will be passed a register resolver to their assembl instruction
    //for each data type that requires resolution, the instruction will pass the data type and their current instruction buffer into the resolver.
    //the resolver may add instructions to the list and then will return what the instruction should use in place of the data
    vector<string> tmpLocalvars;//TODO replace this with real local vars
    RegisterResolver mainResolver(registers, tmpLocalvars);//each stack section gets its own resolver

    vector<FinishedInstruction> finishedInstructions;
    for (auto &instruction: partialInstructions) {
        vector<FinishedInstruction> tmp = instruction->assemble(mainResolver);
        for (auto &instInfo : tmp) {
            finishedInstructions.push_back(std::move(instInfo));
        }
    }

    filesystem::path inFilePath(args[0]);
    std::string outFileName = inFilePath.stem().string();

    ofstream assemblyOut(outFileName+".asm");

    assemblyOut << systemConsts<<endl<<endl;
    if (LCDSystem) {
        assemblyOut << lcdConsts << endl<<endl;
    }

    assemblyOut << "; global vars"<<endl;
    for (size_t i=0;i<globalVars.size();i++) {
        assemblyOut << ".const " << globalVars[i] << "\t" << i << endl;
    }
    assemblyOut << endl;
    for (auto &a: finishedInstructions) {
        cout << a.produce() << endl;//this is probably temporary
        assemblyOut << a.produce() << endl;
    }

    assemblyOut.flush();
    assemblyOut.close();
    cout << "Building completed successfully, assembly file generated" <<endl;

    if (assemble) {
        try {
            cout << "assembling generated assembly" << endl;
            auto tnasm = subprocess::Popen({assembler,outFileName+".asm"},subprocess::output{subprocess::PIPE});
            auto outputBuffer = tnasm.communicate().first;
            cout << outputBuffer.buf.data() << endl;
        } catch (exception& e) {
            cout.flush();
            cerr << "Error while running assembler: " << e.what() << endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
