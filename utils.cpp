#include "utils.hpp" 
#include <stdexcept> 
#include <cctype>    
using namespace std;

FILE* f_ini(string & F, const char* m) {
    FILE* f = fopen(F.c_str(), m);
    if (!f) throw runtime_error("erro ao abrir: " + F);
    return f;
}

string normalize(string & line) {
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
            if (!token.empty()) { 
            if (token != ",") ret.push_back(token); 
            token = ""; 
            }
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