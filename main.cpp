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

struct Error {
    int line;
    string type;
    string message;
};
vector<Error> error_list;

void add_error(int line, string type, string message) {
    error_list.push_back({line, type, message});
}


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
        // caso em que a linha atual define uma macro
        
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
            continue; 
        }
        // caso dentro do corpo de uma macro
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
        if (!tokens.empty() && tokens[0].back() == ':') {
            // se for linha com rótulo
            label = tokens[0];
            aux = (tokens.size() > 1) ? to_upper(tokens[1]) : aux;
        } else if (!tokens.empty()) {
            // se for linha sem rótulo
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
        } else {
            // Se a linha tiver um rótulo printa ele separado em uma linha
            if (!tokens.empty() && tokens[0].back() == ':' && tokens.size() > 1) {
                fprintf(out_file, "%s\n", tokens[0].c_str());
                vector<string> rest_tokens(tokens.begin() + 1, tokens.end());
                fprintf(out_file, "%s\n", join_tokens(rest_tokens).c_str());
            } else if (!tokens.empty()) {
                fprintf(out_file, "%s\n", join_tokens(tokens).c_str());
            }
        }
    } 
    fclose(in_file); 
    fclose(out_file);
}


void single_pass_assembly(string & in, string & out_o1, string & out_o2) {
    FILE* in_file = f_ini(in, "r");
    FILE* out_file1 = f_ini(out_o1, "w");
    FILE* out_file2 = f_ini(out_o2, "w");

    char buffer[MAXB];
    vector<string> lines;
    while (fgets(buffer, sizeof(buffer), in_file)) lines.push_back(string(buffer));

    int counter = 0;

    vector<int> code;
    vector<int> code_pending_list;
    vector<int> code_offsets; // vetor auxiliar para armazenar offsets

    for (int line_number = 0; line_number < (int)lines.size(); line_number++){
        string line = lines[line_number];
        auto tokens = tokenize(line);

        if (tokens.empty()) continue;

        // Verificação léxica de rótulos (nome e posição)

        int label_count = 0;
        for (string &t : tokens) if (t.back() == ':') label_count++;

        // Se houver mais de um rótulo na mesma linha (erro sintático)
        if (label_count > 1) {
            add_error(line_number+1, "SINTÁTICO", "Dois rótulos na mesma linha.");
            continue; 
        }

        // Se o primeiro token é um rótulo
        if (tokens[0].back() == ':') {
            string simbolo = tokens[0];
            simbolo.pop_back(); // remove o ':'

            // Verifica se rótulo começa com número (erro léxico)
            if (isdigit(simbolo[0])) {
                add_error(line_number+1, "LÉXICO", "Rótulo não pode começar com número: " + simbolo);
            }

            // Verifica se o rótulo contém caractere inválido (erro léxico)
            for (char c : simbolo) {
                if (!(isalnum(c) || c == '_')) {
                    add_error(line_number+1, "LÉXICO", "Rótulo contém caractere inválido: " + simbolo);
                    break;
                }
            }

            // Verifica se o rótulo já foi declarado antes (erro semântico)
            if (symbol_table[simbolo].def) {
                add_error(line_number+1, "SEMÂNTICO", "Rótulo declarado duas vezes: " + simbolo);
            }

            // Marca a posição onde ele aparece
            symbol_table[simbolo].val = code.size();
            symbol_table[simbolo].def = true;

            // Resolve pendências anteriores desse símbolo
            int last_seen = symbol_table[simbolo].pending;
            while (last_seen != -1) {
                int temp = code[last_seen];
                code[last_seen] = code.size() + code_offsets[last_seen]; // aplica o offset aqui
                last_seen = temp;
            }

            // passa para próxima linha porque só havia rótulo
            if (tokens.size() == 1) continue;

            // remove o rótulo do vetor para analisar a instrução normalmente
            tokens.erase(tokens.begin());
        }

        // Verificação sintática: instrução inexistente
        string opcode = tokens[0];
        if (!instruction_table.count(opcode) && opcode != "SPACE" && opcode != "CONST") {
            add_error(line_number+1, "SINTÁTICO", "Instrução inexistente: " + opcode);
            continue;
        }

        // Diretivas (SPACE e CONST)
        if (opcode == "SPACE") {
            int space_size = 1; // valor padrão
            
            if (tokens.size() > 2) {
                // Verifica se é o caso especial de número negativo separado
                if (tokens.size() == 3 && tokens[1] == "-" && is_number(tokens[2])) {
                    add_error(line_number+1, "SEMÂNTICO", "SPACE requer um valor positivo.");
                } else {
                    add_error(line_number+1, "SINTÁTICO", "SPACE aceita no máximo 1 argumento.");
                }
            } else if (tokens.size() == 2) {
                if (!is_number(tokens[1])) {
                    add_error(line_number+1, "SINTÁTICO", "SPACE requer um argumento numérico.");
                } else {
                    space_size = stoi(tokens[1]);
                    if (space_size <= 0) {
                        add_error(line_number+1, "SEMÂNTICO", "SPACE requer um valor positivo.");
                        space_size = 1; // usa 1 como fallback
                    }
                }
            }
            
            
            for (int i = 0; i < space_size; i++) {
                code.push_back(0);
                code_offsets.push_back(0);
            }
        }
        else if (opcode == "CONST") {
            if (tokens.size() == 3 && tokens[1] == "-" && is_number(tokens[2])) {
                // Número negativo separado: CONST - 5
                int value = -stoi(tokens[2]);
                code.push_back(value);
                code_offsets.push_back(0);
            } else if (tokens.size() != 2) {
                add_error(line_number+1, "SINTÁTICO", "CONST requer exatamente 1 argumento.");
                code.push_back(0);
                code_offsets.push_back(0);
            } else if (!is_valid_integer(tokens[1])) {
                add_error(line_number+1, "SINTÁTICO", "CONST requer um argumento numérico.");
                code.push_back(0);
                code_offsets.push_back(0);
            } else {
                code.push_back(stoi(tokens[1]));
                code_offsets.push_back(0);
            }
        }

        
        // Instrução comum: verificar número de operandos
        else if (instruction_table.count(opcode)) {
            Instr instruction = instruction_table[opcode];

            // Conta operandos considerando offsets (X + 2 conta como 1 operando)
            // E valida a sintaxe dos offsets durante a contagem
            int operand_count = 0;
            for (int i = 1; i < tokens.size(); i++) {
                operand_count++;
                // Se o próximo token é + ou - seguido de algo
                if (i + 1 < tokens.size() && (tokens[i+1] == "+" || tokens[i+1] == "-")) {
                    if (i + 2 < tokens.size()) {
                        if (is_number(tokens[i+2])) {
                            i += 2; // pula o operador e o número (offset válido)
                        } else {
                            // Offset inválido: X + Y (onde Y não é número)
                            add_error(line_number+1, "SINTÁTICO", 
                                    "Offset deve ser um número, encontrado: " + tokens[i+2]);
                            i += 2; // pula para continuar a análise
                        }
                    } else {
                        // Tem + ou - mas não tem nada depois
                        add_error(line_number+1, "SINTÁTICO", 
                                "Operador " + tokens[i+1] + " sem operando seguinte.");
                    }
                }
            }

            // Verifica número de operandos (apenas reporta erro, não para)
            if (operand_count != instruction.operands) {
                add_error(line_number+1, "SINTÁTICO",
                        "Número incorreto de operandos para " + opcode);
            }

            code.push_back(instruction.opcode);
            code_offsets.push_back(0); // opcode não tem offset

            // Resolve operandos
            int i = 1;
            while (i < tokens.size()) {
                string operand = tokens[i];
                int offset = 0;
                string symbol = operand;
                
                // Verifica se o próximo token é + ou - (offset)
                if (i + 2 < tokens.size() && (tokens[i+1] == "+" || tokens[i+1] == "-")) {
                    if (is_number(tokens[i+2])) {
                        // É um offset válido
                        offset = stoi(tokens[i+2]);
                        if (tokens[i+1] == "-") {
                            offset = -offset;
                        }
                        i += 2; // pula o operador e o número
                    } else {
                        // Offset inválido, já foi reportado, pula mas usa offset 0
                        i += 2;
                    }
                } else if (i + 1 < tokens.size() && (tokens[i+1] == "+" || tokens[i+1] == "-")) {
                    // Operador sem número depois, já foi reportado, ignora o operador
                    i += 1;
                }

                // Se for número puro, só empurra
                if (is_number(symbol)) {
                    code.push_back(stoi(symbol) + offset);
                    code_offsets.push_back(0);
                } 
                // Se for símbolo
                else {
                    // Valida o nome do símbolo
                    if (isdigit(symbol[0])) {
                        add_error(line_number+1, "LÉXICO", "Símbolo não pode começar com número: " + symbol);
                    }
                    
                    bool has_invalid_char = false;
                    for (char c : symbol) {
                        if (!(isalnum(c) || c == '_')) {
                            add_error(line_number+1, "LÉXICO", "Símbolo contém caractere inválido: " + symbol);
                            has_invalid_char = true;
                            break;
                        }
                    }
                    
                    // Se ainda não declarado, deixa pendente
                    if (!symbol_table[symbol].def) {
                        add_error(line_number+1, "SEMÂNTICO", "Rótulo não declarado: " + symbol);
                        code.push_back(symbol_table[symbol].pending);
                        symbol_table[symbol].pending = code.size() - 1;
                        code_offsets.push_back(offset); // guarda o offset para resolver depois
                    } else {
                        // Já declarado, resolve imediatamente
                        code.push_back(symbol_table[symbol].val + offset);
                        code_offsets.push_back(0); // já foi resolvido, não precisa guardar offset
                    }
                }
                
                i++;
            }
        }

        // Atualiza lista de pendências
        for (int i = code_pending_list.size(); i < code.size(); i++){
            code_pending_list.push_back(code[i]);
        }
    }

    // Arquivo .o1: apenas com pendências (sem mostrar offsets intermediários)
    for (int i = 0; i < code_pending_list.size(); i++){
        fprintf(out_file1, "%d", code_pending_list[i]);
        if (i < code_pending_list.size() - 1) fprintf(out_file1, " ");
    }
    fprintf(out_file1, "\n");

    // Arquivo .o2: código final com todos os endereços e offsets resolvidos
    for (int i = 0; i < code.size(); i++){
        fprintf(out_file2, "%d", code[i]);
        if (i < code.size() - 1) fprintf(out_file2, " ");
    }
    fprintf(out_file2, "\n");
    
    fclose(in_file); 
    fclose(out_file1);
    fclose(out_file2);
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
    string err = arq + ".err";

    cout << "teste para compilação do arquivo: " << in << '\n';
    init();
    try {
        pre_proc(in, pre);
        cout << "arquivo .pre gerado...\n";
        single_pass_assembly(pre, o1, o2);
         if (!error_list.empty()) {
            cout << "\nErros encontrados:\n";
            FILE *err_file = fopen(err.c_str(), "w");
            for (auto &e : error_list) {
                cout << "Linha " << e.line << " - " << e.type << ": " << e.message << endl;
                fprintf(err_file, "Linha %d - %s: %s\n", e.line, e.type.c_str(), e.message.c_str());
            }
            fclose(err_file);
            cout << "\nArquivo de erros gerado: " << err << endl;
        } else {
            cout << "\nCompilação concluída sem erros.\n";
        }

    } catch (const exception & erro) {
        cerr << "erro: " << erro.what() << '\n';
        return 1;
    }
    return 0;
}