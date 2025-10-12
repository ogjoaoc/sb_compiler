#include "compiler.hpp"
#include <iostream>
#include <cstdio>
using namespace std;

const int MAXB = 4096;

map<string,Instr> instruction_table;
map<string,Symbol> symbol_table;
map<string,int> directive_table;
map<string,Macro> mnt; 
vector<string> mdt;         

void init() {
    instruction_table = { 
        {"ADD",    {1, 2, 1}},   {"SUB",    {2, 2, 1}},  {"MULT",   {3, 2, 1}},   {"DIV",    {4, 2, 1}},  {"JMP",    {5, 2, 1}},
        {"JMPN",   {6, 2, 1}},   {"JMPP",   {7, 2, 1}},  {"JMPZ",   {8, 2, 1}},   {"COPY",   {9, 3, 2}},  {"LOAD",   {10, 2, 1}},
        {"STORE",  {11, 2, 1}},  {"INPUT", {12, 2, 1}},  {"OUTPUT", {13, 2, 1}},  {"STOP",   {14, 1, 0}}
    };
    directive_table = { 
        {"SPACE", 1}, {"CONST", 1} 
    };
}

string normalize(const string & line) {
    string ret;
    for (char c : line) {
        if (c == '+' || c == '-' || c == ',') {
            if (!ret.empty() && ret.back() != ' ') ret += ' ';
            ret += c;
            ret += ' ';
        } else if (c == ';') {
            break;
        } else {
            ret += c;
        }
    }
    while (!ret.empty() && isspace(ret.back())) ret.pop_back();
    return ret;
};

vector<string> tokenize(const string & line) {
    vector<string> ret;
    string token;
    for (char c : line) {
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            if (!token.empty()) { ret.push_back(token); token = ""; }
        } else {
            token += c;
        }
    }
    if (!token.empty()) ret.push_back(token);
    return ret;
};

string to_upper(string s) {
    for (char &c : s) if (c >= 'a' && c <= 'z') c = c - 'a' + 'A';
    return s;
};

// single pass pre proc
void pre_proc(const string & in, const string & out) {
    FILE* in_file = fopen(in.c_str(), "r");
    if (!in_file) {
        throw runtime_error("erro: arquivo de input no preproc: " + in);
    }
    FILE* out_file = fopen(out.c_str(), "w");
    if (!out_file) {
        fclose(in_file);
        throw runtime_error("erro: arquivo de saida no preproc: " + out);
    }

    auto join_tokens = [&](const vector<string> & tokens) -> string {
        string ret;
        if (!tokens.empty()) {
            ret = tokens[0];
            for (int i = 1; i < (int)tokens.size(); i++) {
                ret += " " + tokens[i];
            }
        }
        return ret;
    };

    char buffer[MAXB];
    bool macro_def = false;
    string cur_label = "", cur_macro_name;
    map<string,vector<string>> macro_args;

    while (fgets(buffer, sizeof(buffer), in_file)) {
        string line(buffer);
        while (!line.empty() && (line.back() == '\n' || line.back() == '\r')) line.pop_back();
        
        vector<string> tokens = tokenize(normalize(line));
        if (tokens.empty()) continue;

        if (macro_def) {
            if (to_upper(tokens[0]) == "ENDMACRO") {
                macro_def = false;
                mdt.push_back("ENDMACRO");
            } else {
                string mdt_line = line;
                auto it = macro_args.find(cur_macro_name);
                if (it != macro_args.end()) {
                    auto &args = it->second;
                    for (int i = 0; i < (int)args.size(); i++) {
                        auto &cur_arg = args[i];
                        string pl = "#" + to_string(i + 1);
                        size_t pos = mdt_line.find(cur_arg);
                        while (pos != string::npos) {
                            mdt_line.replace(pos, cur_arg.length(), pl);
                            pos = mdt_line.find(cur_arg, pos + pl.length());
                        }
                    }
                }
                mdt.push_back(mdt_line);
            }
            continue;
        }

        if (!cur_label.empty()) {
            tokens.insert(tokens.begin(), cur_label.substr(0, cur_label.find(' ')));
            cur_label = "";
        }
        if (tokens.size() == 1 && !tokens[0].empty() && tokens[0].back() == ':') {
            cur_label = tokens[0] + " ";
            continue;
        }

        if (tokens.size() > 1 && to_upper(tokens[1]) == "MACRO") {
            macro_def = true;
            string macro_name = to_upper(tokens[0]);
            macro_name.pop_back();
            cur_macro_name = macro_name;
            
            vector<string> cur_args;
            for (int i = 2; i < (int)tokens.size(); i++) {
                if (tokens[i] != ",") cur_args.push_back(tokens[i]);
            }
            mnt[macro_name] = { (int)cur_args.size(), (int)mdt.size() };
            macro_args[macro_name] = cur_args;
            continue; 
        }

        string wanted_tk, label_part;
        if (!tokens.empty() && tokens[0].back() == ':') {
            label_part = tokens[0];
            if (tokens.size() > 1) wanted_tk = to_upper(tokens[1]);
        } else {
            if (!tokens.empty()) wanted_tk = to_upper(tokens[0]);
        }

        if (!wanted_tk.empty() && mnt.count(wanted_tk)) {
            Macro info = mnt[wanted_tk];
            vector<string> call_args;
            int start_pos = (!label_part.empty()) ? 2 : 1;
            string cur_arg;

            for (int i = start_pos; i < (int)tokens.size(); i++) {
                if (tokens[i] == ",") {
                    if (!cur_arg.empty()) call_args.push_back(cur_arg);
                    cur_arg = ""; 
                } else {
                    if (!cur_arg.empty()) cur_arg += " "; 
                    cur_arg += tokens[i];
                }
            }
            if (!cur_arg.empty()) call_args.push_back(cur_arg);
            
            bool ini_exp = true;
            for (int i = info.mdt_pos; i < (int)mdt.size(); i++) {
                if (to_upper(mdt[i]) == "ENDMACRO") break;
                string expanded = mdt[i];
                for (int j = 0; j < (int)call_args.size(); j++) {
                    string pl = "#" + to_string(j + 1);
                    size_t pos = expanded.find(pl);
                    while (pos != string::npos) {
                        expanded.replace(pos, pl.length(), call_args[j]);
                        pos = expanded.find(pl, pos + call_args[j].length());
                    }
                }
                string clean_line = join_tokens(tokenize(normalize(expanded)));
                if (ini_exp && !label_part.empty()) {
                    fprintf(out_file, "%s %s\n", label_part.c_str(), clean_line.c_str());
                    ini_exp = false;
                } else {
                    fprintf(out_file, "%s\n", clean_line.c_str());
                }
            }
        } else {
            fprintf(out_file, "%s\n", join_tokens(tokens).c_str());
        }
    }
    fclose(in_file);
    fclose(out_file);
}


void single_pass_assembly(const string & in, const string & out_o1, const string & out_o2) {
}


int main(int argc, char *argv[]) {
    if (argc != 2) {
        cerr << "erro: ./compilador <arquivo.asm>" << '\n';
        return 1;
    }
    string in = argv[1];
    string arq = in.substr(0, in.find_last_of('.'));
    string pre = arq + ".pre";
    string o1 = arq + ".o1";
    string o2 = arq + ".o2";
    cout << "teste para compilação do arquivo: " << in << '\n';
    init();
    try {
        pre_proc(in, pre);
        cout << "arquivo .pre gerado...\n";
        // single_pass_assembly(pre, o1, o2);
    } catch (const exception & erro) {
        cerr << "erro: " << erro.what() << '\n';
        return 1;
    }
    return 0;
}