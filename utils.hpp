#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <vector>
#include <cstdio> 
using namespace std;

FILE* f_ini(string & F, const char * m);
string normalize(string & line);
vector<string> tokenize(const string & line);
string to_upper(string s);
#endif 