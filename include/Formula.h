#ifndef FORMULA_H
#define FORMULA_H

#include <string>
#include <vector>

class Formula
{
public:
    std::string left;
    std::vector<std::string> right;

    Formula();                                                   // 默认构造函数声明
    Formula(std::string& left, std::vector<std::string>& right); // 带参数的构造函数声明
    std::vector<std::string> returnRights();
    std::string returnLeft();
};

#endif
