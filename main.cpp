#include "compiler.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstdio>
#include <algorithm>
using namespace std;

const int MAXB = 4096;

map<string,int> directive_table;
map<string,Instr> instruction_table;

map<string,Macro> MNT; 
vector<string> MDT;         

map<string,Symbol> symbol_table;

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

void pre_proc(string & in, string & out) {
    FILE* in_file = f_ini(in, "r");
    FILE* out_file = f_ini(out, "w");
    auto join_tokens = [&](vector<string> & tokens) -> string {
        string ret;
        if (!tokens.empty()) {
            ret = tokens[0];
            for (int i = 1; i < (int)tokens.size(); i++) {
                ret += " " + tokens[i];
            }
        }
        return ret;
    };
    auto replace = [&](string & s, string & a, string & b) {
        size_t it = 0;
        while((it = s.find(a, it)) != string::npos) {
            s.replace(it, (int)a.size(), b);
            it += (int)b.size(); 
        }
    };
    char buffer[MAXB];
    vector<string> lines;
    while (fgets(buffer, sizeof(buffer), in_file)) lines.push_back(string(buffer));
    reverse(lines.begin(), lines.end());

    bool macro_def = false;
    
    while (!lines.empty()) {
        string line = lines.back();
        lines.pop_back();
        vector<string> tokens = tokenize(normalize(line));
        if (tokens.empty()) continue;
        // se for só um rótulo a linha inteira, insere ele na próx linha 
        // ex:  ROTULO:
        //      INPUT N
        if (tokens.size() == 1 && tokens[0].back() == ':') {
            lines.back() = tokens[0] + " " + lines.back();
            continue; 
        }
        // caso em que a linha atual define uma macro
        // ex: "SEILA: MACRO &X"
        bool ini_macro = (tokens.size() > 1 && to_upper(tokens[1]) == "MACRO");
        if (ini_macro) {
            macro_def = true;
            string name = to_upper(tokens[0]); name.pop_back();     
            Macro macro;
            macro.mdt_pos = MDT.size(); // index inicial na MDT
            for (int i = 2; i < tokens.size(); i++) {
                macro.params.push_back(tokens[i]); //  &ARG1 &ARG2 salvos  
            }
            MNT[name] = macro;
            // MNT é sempre nesse formato
            // "SLA_MACRO" -> { mdt_pos: 0, params: ["&X"] }
            continue; 
        }
        // caso de estar dentro do corpo de uma macro
        // só preenche a MDT com a linha crua ou encerra macro
        if (macro_def) {
            if (to_upper(tokens[0]) == "ENDMACRO") {
                macro_def = false;
                MDT.push_back("ENDMACRO");
            } else MDT.push_back(line); 
            continue;
        }
        
        // caso seja chamada de macro ou linha normal
        string label = "", aux = "";
        if (tokens[0].back() == ':') {
            // se for linha com rótulo
            // ex: ["ROTULO:", "SLA_MACRO", "X1"]
            label = tokens[0];
            aux = tokens.size() ? to_upper(tokens[1]) : aux;
        } else {
            // se for linha sem rótulo
            // ex: ["SLA_MACRO", "X1"]
            aux = to_upper(tokens[0]);
        }
        // agora caso aux seja de fato uma chamada de macro
        // tem que expandir
        if (MNT.count(aux)) {
            Macro macro = MNT[aux];
            vector<string> params = macro.params, args, remap;
            // idx é onde começam os argumentos, pode variar dependendo dos casos
            // com rótulo na linha ou sem
            int idx = (label != "") ? 2 : 1; 
            for (int i = idx; i < (int)tokens.size(); i++) {
                args.push_back(tokens[i]);
            }
            // aqui é a parte da expansão, e o remap é só uma mini stack
            // pra guardar as linhas expandidas temporariamente
            // depois as partes que foram expandidas voltam pro vetor lines (sem isso não funciona macro dentro de macro)
            for (int i = macro.mdt_pos; i < (int)MDT.size(); i++) {
                if (to_upper(MDT[i]) == "ENDMACRO") break;
                string exp = MDT[i]; 
                // os parâmetros "&ARGs" são trocados pelos argumentos "Ns"
                for (int j = 0; j < (int)args.size(); j++) {
                    if (j < (int)params.size()) {
                        replace(exp, params[j], args[j]);
                    }
                }
                // se tiver rótulo, coloca ele só na primeira linha da expansão 
                if (i == macro.mdt_pos && label != "") exp = label + " " + exp;
                remap.push_back(exp);
            }
            while(remap.size()) lines.push_back(remap.back()), remap.pop_back();
        } else fprintf(out_file, "%s\n", join_tokens(tokens).c_str());
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