#include <algorithm>
#include <fstream>
#include <iostream>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "LR1Item.cpp"

class SyntaxAnaly
{
    struct OP {
        std::string type;
        int stateNum;
        std::string left;
        std::vector<std::string> right;

        OP(const std::string& type, int num, const std::string& left, const std::vector<std::string>& right)
            : type(type), stateNum(num), left(left), right(right) {}

        std::string toString() const
        {
            if (type == "S")
                return "S" + std::to_string(stateNum);
            else if (type == "R")
                return "R," + left + "->" + getRightAsString();
            else
                return type;
        }

        std::string getRightAsString() const
        {
            std::stringstream ss;
            for (const auto& symbol : right)
                ss << symbol << " ";
            return ss.str();
        }
    };

    std::vector<std::string> GrammarBag;
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> express; // 文法
    std::unordered_map<std::string, std::unordered_set<std::string>> first;         // FIRST集合
    std::unordered_set<std::string> vnSet;                                          // 非终结符Vn集合
    std::unordered_set<std::string> vtSet;                                          // 终结符Vt集合
    std::string start = "Program\'";                                                // 开始结点
    std::string nullStr = "$";                                                      // 空串
    std::string stackEnd = "EOF";
    std::vector<std::set<LR1Item>> states;
    std::vector<std::unordered_map<std::string, OP>> action;
    std::vector<std::unordered_map<std::string, int>> go_to;

public:
    SyntaxAnaly(const std::string& fileName)
    {
        try {
            std::ifstream fileReader(fileName);
            if (fileReader.is_open()) {
                std::string line;
                std::stringstream sb;
                while (std::getline(fileReader, line)) {
                    if (line.empty())
                        continue;
                    sb << line << "\n";
                }
                fileReader.close();
                std::string content = sb.str();
                std::istringstream iss(content);
                std::string temp;
                while (std::getline(iss, temp, '\n')) {
                    GrammarBag.push_back(temp);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception: " << e.what() << std::endl;
        }
        generate();
        generateFirst();
        generateActionGoto();
    }

    void generateActionGoto()
    {
        std::unordered_set<LR1Item> I0;
        LR1Item item0(start, express[start][0], 0, stackEnd);
        // item0.dot = 0;
        // item0.left = start;
        // item0.right = express[start][0];
        // item0.ahead = stackEnd;
        I0.insert(item0);
        I0 = closure(I0, this);
        std::vector<std::unordered_set<LR1Item>> states;
        std::vector<std::unordered_map<std::string, OP>> action;
        std::vector<std::unordered_map<std::string, int>> go_to;
        states.push_back(I0);
        std::string realStart = start.substr(0, start.length() - 1);
        for (int i = 0; i < states.size(); ++i) {
            action.push_back(std::unordered_map<std::string, OP>());
            go_to.push_back(std::unordered_map<std::string, int>());
            std::unordered_set<LR1Item> state = states[i];

            for (auto item : state) {
                if (item.dot == item.right.size()) { // reduce item
                    if (item.left == start && item.right.size() == 1 && item.right[0] == realStart)
                        action[i][item.ahead] = OP{"ACC", -1, "", {}};
                    else
                        action[i][item.ahead] = OP{"R", -1, item.left, item.right};
                } else {
                    std::unordered_set<LR1Item> st = go(state, item.right[item.dot], this);
                    if (states.find(st) != states.end()) {
                        if (vtSet.find(item.right[item.dot]) != vtSet.end())
                            action[i][item.right[item.dot]] = OP{"S", states.find(st), "", {}};
                        else
                            go_to[i][item.right[item.dot]] = states.find(st);
                    } else {
                        if (vtSet.find(item.right[item.dot]) != vtSet.end())
                            action[i][item.right[item.dot]] = OP{"S", states.size(), "", {}};
                        else
                            go_to[i][item.right[item.dot]] = states.size();
                        states.push_back(st);
                    }
                }
            }
        }
    }

    void generate()
    {
        int i = 0;
        for (const std::string& str : GrammarBag) {
            // 文法箭头左边
            std::string vnString = trim(str.substr(0, str.find("->")));
            // 文法箭头右边
            std::string string = trim(str.substr(str.find("->") + 2));

            if (i == 0) {
                start = vnString;
                i++;
            }
            // 生成可识别产生式
            std::vector<std::vector<std::string>> list = express.count(vnString) ? express[vnString]
                                                                                 : std::vector<std::vector<std::string>>();
            std::vector<std::string> right;
            std::string::size_type startPos = 0;
            std::string::size_type endPos = string.find(" ");
            while (endPos != std::string::npos) {
                right.push_back(string.substr(startPos, endPos - startPos));
                startPos = endPos + 1;
                endPos = string.find(" ", startPos);
            }
            right.push_back(string.substr(startPos));
            list.push_back(right);
            express[vnString] = list;
            // 生成非终结符集合
            vnSet.insert(vnString);
        }

        // 生成终结符集
        for (const std::string& vnStr : vnSet) {
            for (const std::vector<std::string>& right : express[vnStr]) {
                for (const std::string& s : right) {
                    if (vnSet.find(s) == vnSet.end()) {
                        if (s.empty()) {
                            std::cout << "";
                        }
                        vtSet.insert(s);
                    }
                }
            }
        }
        vtSet.erase(nullStr);
    }

    void generateFirst()
    {
        for (const std::string& vt : vtSet) {
            std::unordered_set<std::string> st;
            st.insert(vt);
            first[vt] = st;
        }
        for (const std::string& vn : vnSet) {
            std::unordered_set<std::string> st;
            first[vn] = st;
        }
        bool flag = true;
        while (flag) {
            flag = false;
            for (const std::string& vn : vnSet) {
                std::size_t size = first[vn].size();
                for (const std::vector<std::string>& right : express[vn]) {
                    auto rightFirst = getRightFirst(right);
                    first[vn].insert(rightFirst.begin(), rightFirst.end());
                }
                flag |= size < first[vn].size();
            }
        }
    }

    std::unordered_set<std::string> getRightFirst(const std::vector<std::string>& right)
    {
        std::unordered_set<std::string> res;

        if (vtSet.count(right[0])) { // 如果这个产生式是 vt 的话
            res.insert(right[0]);
        } else if (right.size() == 1 && right[0] == nullStr) { // 如果这个产生式是 X -> epsilon 的话
            res.insert(nullStr);
        } else if (vnSet.count(right[0])) {                 // 如果产生式是 vn 的话
            for (const std::string& vt : first[right[0]]) { // 将第一个非终结符的first集合中的非epsilon的终结符加进去
                if (vt != nullStr)
                    res.insert(vt);
            }

            std::size_t i = 0;
            for (; i < right.size(); ++i) {
                if (first[right[i]].count(nullStr) == 0)
                    break;
            }
            if (i == right.size()) { // 如果这个产生式右边的first集合都有epsilon，那么把epsilon加进去
                res.insert(nullStr);
            } else {
                for (const std::string& vt : first[right[i]]) {
                    if (vt != nullStr)
                        res.insert(vt);
                }
            }
        }
        return res;
    }

    void analyze(const std::string& s, std::ofstream& writer)
    {
        std::string input = s;
        input = trim(input);
        input += " " + stackEnd;
        std::stack<int> state_stack;
        std::stack<std::string> v_stack;
        state_stack.push(0);
        v_stack.push(stackEnd);
        std::vector<std::string> tokens = splitString(input, ' ');
        int num = 1;
        for (int i = 0; i < tokens.size(); ++i) {
            std::string token = tokens[i];
            int now_state = state_stack.top();
            OP act = action[now_state][token];
            if (writer.is_open())
                writer << num << "\t" << v_stack.top() << "#" << token << "\t";
            std::cout << num << "\t" << v_stack.top() << "#" << token << "\t";
            ++num;
            if (act.type == "S") {
                state_stack.push(act.stateNum);
                v_stack.push(token);
            } else if (act.type == "R") {
                for (int j = 0; j < act.right.size(); ++j) {
                    v_stack.pop();
                    state_stack.pop();
                }
                state_stack.push(go_to[state_stack.top()][act.left]);
                v_stack.push(act.left);
                --i;
            } else if (act.type == "ACC") {
            }
            if (act ==) {
                if (writer.is_open())
                    writer << "ERROR"
                           << "\n";
                std::cout << "ERROR" << std::endl;
                return;
            } else if (act.type == "S") {
                if (writer.is_open())
                    writer << "move"
                           << "\n";
                std::cout << "move" << std::endl;
            } else if (act.type == "R") {
                if (writer.is_open())
                    writer << "reduction"
                           << "\n";
                std::cout << "reduction" << std::endl;
            } else {
                if (writer.is_open())
                    writer << "accept"
                           << "\n";
                std::cout << "accept" << std::endl;
            }
        }
    }

private:
    std::string trim(const std::string& s)
    {
        std::string result = s;
        result.erase(result.begin(), std::find_if(result.begin(), result.end(), [](int ch) {
                         return !std::isspace(ch);
                     }));
        result.erase(std::find_if(result.rbegin(), result.rend(), [](int ch) {
                         return !std::isspace(ch);
                     }).base(),
                     result.end());
        return result;
    }

    std::vector<std::string> splitString(const std::string& input, char delimiter)
    {
        std::vector<std::string> tokens;
        std::istringstream iss(input);
        std::string token;

        while (std::getline(iss, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }
};
