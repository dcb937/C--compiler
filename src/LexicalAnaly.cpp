#include "LexicalAnaly.h"
using namespace std;

LexicalAnaly::LexicalAnaly()
{
    initializeDFA();
    lex_result_stack.clear();
}

void LexicalAnaly::performLexicalAnalysis()
{
    // initializeDFA(ch);

    ifstream input("testcases/code.txt");   // 打开输入文件
    ofstream output("output/output02.txt"); // 打开输出文件
    ofstream output2("output/IntermediateResults.txt");
    if (!input) {
        std::cerr << "无法打开文件 "
                  << "testcases/code.txt" << std::endl;
        exit(-1);
    }
    if (!output) {
        std::cerr << "无法打开文件 "
                  << "output/output02.txt" << std::endl;
        exit(-1);
    }
    if (!output2) {
        std::cerr << "无法打开文件 "
                  << "output/IntermediateResults.txt" << std::endl;
        exit(-1);
    }
    unordered_map<string, TokenType> keywords = {{"int", KW}, {"void", KW}, {"return", KW}, {"const", KW}};                                                                                                        // 关键字表
    unordered_map<string, TokenType> operators = {{"+", OP}, {"-", OP}, {"*", OP}, {"/", OP}, {"%", OP}, {"=", OP}, {">", OP}, {"<", OP}, {"==", OP}, {">=", OP}, {"<=", OP}, {"!=", OP}, {"&&", OP}, {"||", OP}}; // 运算符表
    unordered_map<string, TokenType> separators = {{"(", SE}, {")", SE}, {"{", SE}, {"}", SE}, {";", SE}, {",", SE}};                                                                                              // 界符表
    unordered_map<string, TokenType> opse = {{"+", OP}, {"-", OP}, {"*", OP}, {"/", OP}, {"%", OP}, {"=", OP}, {">", OP}, {"<", OP}, {"==", OP}, {">=", OP}, {"<=", OP}, {"!=", OP}, {"&&", OP}, {"||", OP}, {"(", SE}, {")", SE}, {"{", SE}, {"}", SE}, {";", SE}, {",", SE}};
    unordered_map<string, Symbol> symbolTable; // 符号表
    string line;
    int lineNum = 1;
    while (getline(input, line)) {
        int colNum = 1;

        int currentState = 0;
        for (int i = 0; i < line.length(); i++) {
            string idn;
            idn += line[i];
            if (isspace(line[i])) { // 跳过空格和制表符
                colNum++;
                continue;
            }
            if (isalpha(line[i]) || line[i] == '_') { // 处理标识符
                while (i + 1 < line.length() && (isalnum(line[i + 1]) || line[i + 1] == '_')) {
                    i++;
                    idn += line[i];
                }
                if (keywords.count(idn)) { // 如果是关键字
                    currentState = DFA[currentState].transitions[4];
                    output << idn << "\t<" << type[DFA[currentState].type] << ">\n";
                    output2 << idn << "\n";
                    lex_result_stack.push_back(idn);
                    // cout<<idn<<endl;
                } else {                          // 否则是标识符
                    if (symbolTable.count(idn)) { // 如果已经在符号表中
                        currentState = 0;
                        currentState = DFA[currentState].transitions[5];
                        output << idn << "\t<" << type[DFA[currentState].type] << "> \n"; // 标记为符号表中标识符的类型
                        output2 << "Ident"
                                << "\n";
                        lex_result_stack.push_back("Ident");
                    } else { // 否则添加到符号表中
                        Symbol symbol = {idn, IDN, lineNum, colNum};
                        symbolTable[idn] = symbol;
                        currentState = 0;
                        currentState = DFA[currentState].transitions[5];
                        output << idn << "\t<" << type[DFA[currentState].type] << "> \n";
                        output2 << "Ident"
                                << "\n";
                        lex_result_stack.push_back("Ident");
                    }
                }
            } else if (isdigit(line[i]) || line[i] == '-') { // 处理整数或未知字符
                string abc;
                bool isNegative = line[i] == '-';
                int a = i;
                while (i + 1 < line.length() && isdigit(line[a + 1]) && a < line.length()) {
                    i++;
                    idn += line[i];
                    abc = line[i + 1];
                    if (operators.count(abc) || separators.count(abc) || line[i + 1] == ' ') {
                        a = line.length();
                    }
                    // cout<<a<<"/t"<<idn<<endl;
                }
                bool has_alpha_or_underscore = false;
                for (int j = a + 1; j < line.length(); j++) {
                    if (isalpha(line[j]) || line[j] == '_') {
                        has_alpha_or_underscore = true;
                        break;
                    }
                }
                if (isNegative && !isdigit(line[i + 1])) { // 如果是减号运算符
                    // ch=2;
                    currentState = 0;
                    currentState = DFA[0].transitions[2];
                    output << idn << "\t<" << type[currentState] << ">\n";
                    output2 << idn << "\n";
                    lex_result_stack.push_back("Ident");
                    // output <<  << "\t<OP>\n";
                } else if (isNegative && isdigit(line[a + 1]) && has_alpha_or_underscore) { // 如果是未知字符
                    // ch=2;
                    currentState = 0;
                    currentState = DFA[0].transitions[6];
                    output << idn << "\t<" << type[currentState] << ">\n";
                    output2 << "UNK"
                            << "\n";
                    lex_result_stack.push_back("UNK");
                    // output <<  << "\t<OP>\n";

                } else if (!isNegative && has_alpha_or_underscore && isdigit(a)) { // 如果是未知字符
                    // ch=2;
                    currentState = 0;
                    currentState = DFA[0].transitions[6];
                    output << idn << "\t<" << type[currentState] << ">\n";
                    output2 << "UNK"
                            << "\n";
                    lex_result_stack.push_back("UNK");
                    // output <<  << "\t<OP>\n";

                } else { // 否则是整数
                    // ch=1;
                    currentState = 0;
                    currentState = DFA[currentState].transitions[1];
                    output << idn << "\t<" << type[currentState] << ">\n";
                    output2 << "INT"
                            << "\n";
                    lex_result_stack.push_back("INT");
                    // output << num << "\t<INT>\n";
                }

            } else { // 处理运算符和界符

                if (i + 1 < line.length() && operators.count(idn + line[i + 1])) {
                    idn += line[i + 1];
                    i++;
                }
                if (operators.count(idn)) { // 如果是运算符
                    // ch=2;
                    currentState = 0;
                    currentState = DFA[currentState].transitions[2];
                    output << idn << "\t<" << type[currentState] << ">\n";
                    output2 << idn << "\n";
                    lex_result_stack.push_back(idn);
                    // output << op << "\t<OP>\n";
                } else if (separators.count(idn)) { // 如果是界符
                    // ch=3;
                    currentState = 0;
                    currentState = DFA[currentState].transitions[3];
                    output << idn << "\t<" << type[currentState] << ">\n";
                    output2 << idn << "\n";
                    lex_result_stack.push_back(idn);
                    // output << op << "\t<SE>\n";
                } else { // 否则是未知字符
                    output << idn << "\t<UNK>\n";
                    output2 << "UNK"
                            << "\n";
                    lex_result_stack.push_back("UNK");
                }
            }
            colNum++;
        }
        lineNum++;
    }
    // 输出符号表
    /*
    output << "\n\nSymbol Table:\n";
    for (auto iter = symbolTable.begin(); iter != symbolTable.end(); iter++) {
        output << iter->first << "\t<" << iter->second.type << ", " << iter->second.line << ", " << iter->second.column << ">\n";
    }
    */
    input.close();   // 关闭输入文件
    output.close();  // 关闭输出文件
    output2.close(); // 关闭输出文件
}

void LexicalAnaly::initializeDFA()
{
    // 初始化 DFA 状态数组
    // 状态 0
    DFA[0].transitions[1] = 1;
    DFA[0].transitions[2] = 2;
    DFA[0].transitions[3] = 3;
    DFA[0].transitions[4] = 4;
    DFA[0].transitions[5] = 5;
    DFA[0].transitions[6] = 6;
    //初始状态
    DFA[0].type = TokenType::START;

    // 状态 1，整数
    DFA[1].type = TokenType::INT;
    // 状态 2，运算符
    DFA[2].type = TokenType::OP;
    // 状态 3，界符
    DFA[3].type = TokenType::SE;
    // 状态 4，关键字
    DFA[4].type = TokenType::KW;
    // 状态 5，标识符
    DFA[5].type = TokenType::IDN;
    // 状态 6，位置字符
    DFA[6].type = TokenType::UNK;
}

vector<string> LexicalAnaly::getLex_result_stack()
{
    return lex_result_stack;
}