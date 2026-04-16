#include <iostream>
#include <vector>
#include <fstream>
#include "common.hpp"
#include "lcdHighLevel.hpp"

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

//the default mode of this lang is for the LCD system, the edison system, will be usable by a flag

//this process pipeline include several stages:
/* 1. Expansion - expand macro function into their required assembly, this includes thing like rect or variable assignment. this stage includes no hard registers
 * 2. variable assignment - all values except immediate get represented by a variable who gets an assigned memory address or stack address
 * 3. cached register assignment - all variables and required immediate are assigned to registers for usage in a style where registers are treated as a fully associated cache
*/

//each arrow here represents one of the stages above
// hi level built in functions and constructs -> partial instructions -> unfinished assembly -> finished assembly

//this lang will feature 2 types of variables, global heap vars and local stack vars
//there will also be user defined constants in a global scope






vector<Register> registers;

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
    }


    return EXIT_SUCCESS;
}
