#ifndef SYNTAXANALY_H
#define SYNTAXANALY_H

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

class SyntaxAnaly
{
public:
    std::vector<Formula> formulas;
    std::vector<std::string> terminals;
    std::vector<std::string> nonTerminals;
    std::unordered_map<std::string, std::vector<std::string>> firsts;
    std::unordered_map<std::string, std::vector<std::string>> follows;
    std::unordered_map<std::string, Formula> predictions;

    void writeAllIntoFile();
    void setFormulas();
    void setNonTerminals();
    void setTerminals();
    void Do();
};

#endif
