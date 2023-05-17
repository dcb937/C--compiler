#include "Formula.h"

#include "utils.h"

using namespace std;

void setFirst(vector<Formula>& formulas, vector<string>& terminals,
              vector<string>& nonTerminals, unordered_map<string, vector<string>>& firsts)
{
    // 终结符全部求出first集
    vector<string> first;
    for (const string& terminal : terminals) {
        first = {terminal};
        firsts[terminal] = first;
    }

    // 给所有非终结符注册一下
    for (const string& nonterminal : nonTerminals) {
        first = {};
        firsts[nonterminal] = first;
    }

    bool flag;
    while (true) {
        flag = true;
        string left;
        string right;
        vector<string> rights;

        // 遍历所有文法
        for (Formula& formula : formulas) {
            left = formula.left;
            rights = formula.returnRights();

            // 每个文法的右侧
            for (const string& s : rights) {
                right = s;

                // oneOfRight是否存在，遇到空怎么办
                if (right != "$") { // 右侧的字符串不为空（任意一个都不为空）
                    // 遍历每一个右侧字符串的First集合，即 First(右侧字符串)
                    // 这个就类似于一个递归
                    for (int l = 0; l < firsts[right].size(); l++) {
                        // First(left) 包括了 First(oneOfRights)
                        if (find(firsts[left].begin(), firsts[left].end(), firsts[right][l]) != firsts[left].end()) {

                            continue;
                        } else {
                            // 不包括, 就加入
                            firsts[left].push_back(firsts[right][l]);
                            flag = false;
                        }
                    }
                }

                // OneOfRights -> $
                if (isCanBeNull(formulas, right)) {
                    continue;
                } else {
                    break;
                }
            }
        }

        if (flag) {
            break;
        }
    }
}

// 判断是否产生$
bool isCanBeNull(vector<Formula>& formulas, string& symbol)
{
    vector<string> rights;

    // 遍历每一个文法
    for (Formula& formula : formulas) {
        // 找到产生式
        if (formula.left == symbol) {
            // symbol -> [rights]
            rights = formula.returnRights();

            // symbol -> $
            if (rights[0] == "$") { // 第一个就是$, 即类似于 E -> $
                return true;
            }
        }
    }

    return false;
}

void setFollow(vector<Formula>& formulas, vector<string>& terminals,
               vector<string>& nonTerminals, unordered_map<string, vector<string>>& firsts,
               unordered_map<string, vector<string>>& follows)
{
    // 所有非终结符的follow集初始化一下
    for (const string& nonTerminal : nonTerminals) {
        vector<string> follow;
        follows[nonTerminal] = follow;
    }
    // 将#加入到follow(S)中
    follows["program"].push_back("#"); // "program": 入口文法！

    bool flag;
    bool fab;
    while (true) {
        flag = true;
        // 循环
        for (Formula& formula : formulas) {
            string left;
            string right;
            vector<string> rights = formula.returnRights();
            for (int j = 0; j < rights.size(); j++) {
                right = rights[j];

                // 非终结符
                if (find(nonTerminals.begin(), nonTerminals.end(), right) != nonTerminals.end()) {
                    fab = true;
                    for (int k = j + 1; k < rights.size(); k++) {
                        // 查找first集
                        for (int v = 0; v < firsts[rights[k]].size(); v++) {
                            // 将后一个元素的first集加入到前一个元素的follow集中
                            if (find(follows[right].begin(), follows[right].end(), firsts[rights[k]][v]) != follows[right].end()) {
                                continue;
                            } else {
                                follows[right].push_back(firsts[rights[k]][v]);
                                flag = false;
                            }
                        }
                        if (isCanBeNull(formulas, rights[k])) {
                            continue;
                        } else {
                            fab = false;
                            break;
                        }
                    }
                    if (fab) {
                        left = formula.returnLeft();
                        for (int p = 0; p < follows[left].size(); p++) {
                            if (find(follows[right].begin(), follows[right].end(), follows[left][p]) != follows[right].end()) {
                                continue;
                            } else {
                                follows[right].push_back(follows[left][p]);
                                flag = false;
                            }
                        }
                    }
                }
            }
        }
        if (flag) {
            break;
        }
    }

    // 清除follow集中的#
    string left;
    for (const string& nonterminal : nonTerminals) {
        left = nonterminal;
        for (int v = 0; v < follows[left].size(); v++) {
            if (follows[left][v] == "#") {
                follows[left].erase(follows[left].begin() + v);
                break;
            }
        }
    }

    // -------------------
    // 为Follow加上#
    for (const string& notTerminal : nonTerminals) {
        follows[notTerminal].push_back("#");
    }
    // -------------------
}

void setPrediction(vector<Formula>& formulas, vector<string>& terminals,
                   vector<string>& nonTerminals, unordered_map<string, vector<string>>& firsts,
                   unordered_map<string, vector<string>>& follows,
                   unordered_map<string, Formula>& predictions)
{
    // (2)
    for (Formula& formula : formulas) {
        // First(formula.right[0])
        try {
            if (formula.right[0] == "$") { // 类似于 value' -> $ 这种文法
                // First集合中First($)是不存在的
                // 遇到这种文法，直接跳过就行
                continue;
            }
            for (const string& terminalInFirsts : firsts[formula.right[0]]) {
                // 空
                if (terminalInFirsts == "$") {
                    // Follow(formula.left)
                    for (const string& terminalInFollows : follows[formula.left]) {
                        std::vector<std::string> tmp = {"$"};
                        predictions[getMapKey(terminalInFollows, formula.left)] = Formula(formula.left, tmp);
                    }
                }
                // 不空
                // [Terminal, notTerminal] : formula
                predictions[getMapKey(terminalInFirsts, formula.left)] = formula;
            }
        } catch (const exception& e) {
            cout << "first集合中没有 key: " << formula.right[0] << endl;
            e.what();
        }
    }

    // (3)
    // E -> $
    for (Formula& formula : formulas) {
        if (formula.returnRights()[0] == "$") {                                 // E -> $
            for (const string& followElement : follows[formula.returnLeft()]) { // Follow(E)
                // [FollowElement(E), E] : E - > $
                predictions[getMapKey(followElement, formula.returnLeft())] = formula;
            }
        }
    }
}

// 以固定的格式产生分析表的 Key
string getMapKey(const string& terminal, const string& nonTerminal)
{
    // i 为终结符，横坐标
    // j 为非终结符，纵坐标
    return "{横坐标: " + terminal + "  " + "纵坐标: " + nonTerminal + "}";
}
