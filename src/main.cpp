#include <algorithm>
#include <cstdlib>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using namespace std;

#include "Formula.h"
#include "LexicalAnaly.h"
#include "SyntaxAnaly.h"
#include "utils.h"

unordered_map<string, Formula> predictMap; // 预测表
vector<string> input_str;                  // 输入串, 词法分析的结果
vector<string> symbol_stack;               // 符号栈
vector<string> parse_result_stack;         // 语法分析输出展示的结果
int parse_result_counter;                  // 语法分析输出结果的计数器

const string parseResultPath = "output/gra.txt";
const string lexiconMiddleResult = "词法分析产生的中间结果.txt";

// 将词法分析传递给语法分析的中间结果打印出来
void writeLexiconMiddleResultIntoFile()
{
    ofstream outfile(lexiconMiddleResult);
    for (const auto& str : input_str) {
        outfile << str << "\n";
    }
    outfile.close();
}

// 利用预测表和词法分析的输入来解析
// 语法分析
void parse()
{
    // ------
    // 将 # 压入栈
    symbol_stack.push_back("#");
    input_str.push_back("#"); //输入串在最后也需要放 #
    // ------

    // 初始符号压入栈
    symbol_stack.push_back("program"); // 入口文法

    string predictMapKey; // PredictMap-Key

    // 符号栈和输入串如果同时为0，那么语法分析结束
    while (true) {
        parse_result_counter++; // 语法分析结果的计数器加一
        if (symbol_stack.back() == "#" && input_str.front() == "#") {
            parse_result_stack.push_back(to_string(parse_result_counter) + "\t" + "EOF" + "#" + "EOF" + "\t" + "accept");
            break;
        }
        // 输入缓冲区与推导符号串第一个字符相等的话，删掉
        if (input_str.front() == symbol_stack.back()) {
            // 语法分析的结果写入栈中
            parse_result_stack.push_back(to_string(parse_result_counter) + "\t" + symbol_stack.back() + "#" + input_str.front() + "\t" + "move");
            input_str.erase(input_str.begin()); // 输入字符移除第一个，类似于指针向后遍历
            symbol_stack.pop_back();            // 符号栈移除栈顶
            continue;
        }

        // 匹配字符
        predictMapKey = getMapKey(input_str.front(), symbol_stack.back());

        // 能够找到匹配的
        if (predictMap.find(predictMapKey) != predictMap.end()) { // 文法不为空，为空报错
                                                                  // 语法分析的结果写入栈中
            parse_result_stack.push_back(to_string(parse_result_counter) + "\t" + symbol_stack.back() + "#" + input_str.front() + "\t" + "reduction");
            // 符号栈的最后一个元素如果是 #, 就不能删除了
            if (symbol_stack.back() != "#") {
                symbol_stack.pop_back(); // 删除符号栈中最后一个元素
            }
            Formula formula = predictMap[predictMapKey];    // 找到文法
            vector<string> rights = formula.returnRights(); // 文法的右侧
            if (rights[0] != "$") {                         // E->$，不能压入空
                for (int i = rights.size() - 1; i >= 0; i--) {
                    // 将文法右侧的非终结符反向压入栈中
                    symbol_stack.push_back(rights[i]);
                }
            }
        } else {
            // 语法分析的结果写入栈中
            parse_result_stack.push_back(to_string(parse_result_counter) + "\t" + symbol_stack.back() + "#" + input_str.front() + "\t" + "error");
            return; // 遇到error直接返回
        }
    }
}

// 输出语法分析结果
void printParseResult()
{
    cout << "开始输出语法分析结果: --------------------" << endl;
    for (const auto& s : parse_result_stack) {
        cout << s << endl;
    }
    ofstream outfile(parseResultPath);
    for (const auto& s : parse_result_stack) {
        outfile << s << "\n";
    }
    outfile.close();
}

// 入口函数
int main()
{
    system("chcp 65001"); // 解决中文乱码

    SyntaxAnaly Syntax;
    LexicalAnaly Lexical;

    Lexical.performLexicalAnalysis();
    input_str = Lexical.getLex_result_stack(); // TODO: 词法分析的输入
    //  cout << input_str[0] << " " << input_str[1] << endl;
    //  // READ IntermediateResults.txt BEGIN
    //  // 由于词法代码没有实现，暂使用软工的代码生成的中间结果作为语法的输入
    //  // 为了和软工输入保持统一，grammar.txt文件使用的是软工的，不是我们的大作业提供的，后期应当替换
    // string filename = "IntermediateResults.txt";
    // std::ifstream infile(filename); // 打开文件

    // if (!infile) {
    //     std::cerr << "无法打开文件 " << filename << std::endl;
    //     return 1;
    // }

    // std::string line;
    // while (std::getline(infile, line)) { // 逐行读取文件内容
    //     input_str.push_back(line);       // 将每行内容存储到 input_str 中
    // }

    // infile.close(); // 关闭文件
    // // READ IntermediateResults.txt END

    symbol_stack.clear();
    parse_result_stack.clear();
    parse_result_counter = 0;

    Syntax.Do();                     // 生成各种表，First，Follow，预测表
    predictMap = Syntax.predictions; // 预测表

    Syntax.writeAllIntoFile(); // 将语法分析开始前生成的所有表打印出来
    // writeLexiconMiddleResultIntoFile(); // 将词法分析的中间结果打印出来

    parse(); // 开始语法分析

    printParseResult(); // 打印语法分析结果
    cout << "语法分析结束" << endl;
    return 0;
}
