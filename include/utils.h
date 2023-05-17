#ifndef UTILS_H
#define UTILS_H

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

bool isCanBeNull(std::vector<Formula>& formulas, std::string& symbol);
void setFirst(std::vector<Formula>& formulas, std::vector<std::string>& terminals,
              std::vector<std::string>& nonTerminals, std::unordered_map<std::string, std::vector<std::string>>& firsts);
void setFollow(std::vector<Formula>& formulas, std::vector<std::string>& terminals,
               std::vector<std::string>& nonTerminals, std::unordered_map<std::string, std::vector<std::string>>& firsts,
               std::unordered_map<std::string, std::vector<std::string>>& follows);
void setPrediction(std::vector<Formula>& formulas, std::vector<std::string>& terminals,
                   std::vector<std::string>& nonTerminals, std::unordered_map<std::string, std::vector<std::string>>& firsts,
                   std::unordered_map<std::string, std::vector<std::string>>& follows,
                   std::unordered_map<std::string, Formula>& predictions);
std::string getMapKey(const std::string& terminal, const std::string& nonTerminal);

#endif
