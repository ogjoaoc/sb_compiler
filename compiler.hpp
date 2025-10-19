#ifndef COMPILER_HPP
#define COMPILER_HPP
#include <string>
#include <vector>
#include <map>
using namespace std;

struct Instr {
    int opcode, sz;
    int operands;
};
struct Symbol {
    int val, pending_list_head; 
    bool def;
};
struct Macro {
    int mdt_pos;
    vector<string> params; // &ARG1 e &ARG2
};

extern map<string,Instr> instruction_table;
extern map<string,Symbol> symbol_table;
extern map<string,int> directive_table;

// estruturas pra lidar com os macros
extern map<string,Macro> mnt; 
extern vector<string> mdt;   

// main
void init();
void pre_proc(const string & in, const string & out);
void single_pass_assembly(const string & in, const string & out_o1, const string & out_o2);
#endif 