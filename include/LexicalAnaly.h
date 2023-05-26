#ifndef LEXICALANALY_H
#define LEXICALANALY_H
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// 定义词法单元的类型

enum TokenType {
    START,//初始态
    INT ,// 整数
    OP, // 运算符
    SE, // 界符
    KW, // 关键字
    IDN,// 标识符
    UNK,//未知字符
    TH   
};
// 定义状态结构体
struct State {
   // std::string name;
    std::unordered_map<int, int> transitions;
    //unordered_map<int, int> transitions;  // 状态转移表
    TokenType type;                        // 状态对应的词法单元类型
};
// 定义符号表的结构体
struct Symbol {
    std::string name; // 标识符的名称
    TokenType type; // 标识符的类型
    int line; // 标识符第一次出现的行数
    int column; // 标识符第一次出现的列数
};
class LexicalAnaly
{
public:
    const std::string type[8]={
    "START",//初始态
    "INT",// 整数
    "OP", // 运算符
    "SE", // 界符
    "KW", // 关键字
    "IDN",// 标识符
    "UNK",//未知字符
    "TH"
    };

    LexicalAnaly();

    void performLexicalAnalysis();

    std::vector<std::string> getLex_result_stack();

private:
    std::vector<std::string> lex_result_stack;
    State DFA[8]; // DFA 状态数组
    void initializeDFA();
};

#endif