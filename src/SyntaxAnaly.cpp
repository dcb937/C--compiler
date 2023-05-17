#include "Formula.h"
#include "utils.h"

#include "SyntaxAnaly.h"
using namespace std;

const string formulaPath = "output/formula.txt";
const string terminalPath = "output/terminal.txt";
const string nonTerminalPath = "output/nonterminal.txt";
const string firstTablePath = "output/first.txt";
const string followTablePath = "output/follow.txt";
const string predictMapPath = "output/predict.txt";

const string grammarPath = "grammar.txt";

void SyntaxAnaly::writeAllIntoFile()
{
    ofstream out(formulaPath);
    for (const Formula& formula : formulas) {
        out << "文法左侧: " << formula.left << "\t"
            << "文法右侧: ";
        for (const string& right : formula.right) {
            out << right << " ";
        }
        out << "\n";
    }
    out.close();

    ofstream out2(terminalPath);
    for (const string& s : terminals) {
        out2 << s << "\n";
    }
    out2.close();

    ofstream out3(nonTerminalPath);
    for (const string& s : nonTerminals) {
        out3 << s << "\n";
    }
    out3.close();

    ofstream out4(firstTablePath);
    for (const auto& entry : firsts) {
        out4 << entry.first << "\t";
        for (const string& s : entry.second) {
            out4 << s << " ";
        }
        out4 << "\n";
    }
    out4.close();

    ofstream out5(followTablePath);
    for (const auto& entry : follows) {
        out5 << entry.first << "\t";
        for (const string& s : entry.second) {
            out5 << s << " ";
        }
        out5 << "\n";
    }
    out5.close();

    ofstream out6(predictMapPath);
    for (const auto& entry : predictions) {
        out6 << entry.first << "\t"
             << "文法: " << entry.second.left << "->";
        for (const string& right : entry.second.right) {
            out6 << right << " ";
        }
        out6 << "\n";
    }
    out6.close();
}

void SyntaxAnaly::setFormulas()
{
    ifstream file(grammarPath);
    string line;
    while (getline(file, line)) {
        string left = line.substr(0, line.find("->") - 1);
        string right = line.substr(line.find("->") + 2);
        size_t startPos = right.find_first_not_of(' ');
        if (startPos != std::string::npos) {
            right.erase(0, startPos);
        }

        vector<string> rightVec;
        size_t pos = 0;
        while ((pos = right.find(" ")) != string::npos) {
            string token = right.substr(0, pos);
            rightVec.push_back(token);
            right.erase(0, pos + 1);
        }
        if (!right.empty()) {
            rightVec.push_back(right);
        }
        Formula formula(left, rightVec);
        formulas.push_back(formula);
    }
    file.close();
}

void SyntaxAnaly::setNonTerminals()
{
    for (Formula formula : formulas) {
        if (find(nonTerminals.begin(), nonTerminals.end(), formula.left) != nonTerminals.end()) {
            continue;
        } else {

            nonTerminals.push_back(formula.left);
        }
    }
}

void SyntaxAnaly::setTerminals()
{
    // 不知道为什么上面处理的多了一个空格在最后。。。
    for (string& s : nonTerminals) {
        size_t lastNonSpace = s.find_last_not_of(" ");
        if (lastNonSpace != std::string::npos) {
            s.erase(lastNonSpace + 1);
        }
    }
    // for (char c : nonTerminals[0]) {
    //     std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)(unsigned char)c << " ";
    // }
    for (const Formula& formula : formulas) {
        vector<string> rights = formula.right;
        for (const string& s : rights) {

            if (find(nonTerminals.begin(), nonTerminals.end(), s) != nonTerminals.end() || s == "$" || s == "") {
                continue;
            } else {
                // cout << "s: " << s << "  len: " << s.length() << endl;
                // cout << "find(nonTerminals.begin(), nonTerminals.end(), s): " << distance(nonTerminals.begin(), find(nonTerminals.begin(), nonTerminals.end(), s)) << endl;
                // cout << "find(nonTerminals.begin(), nonTerminals.end(), s) != nonTerminals.end() : " << (find(nonTerminals.begin(), nonTerminals.end(), s) != nonTerminals.end()) << endl;
                // cout << "s == \"$\": " << (s == "$") << endl;
                terminals.push_back(s);
            }
        }
    }
}

void SyntaxAnaly::Do()
{
    formulas.clear();
    terminals.clear();
    nonTerminals.clear();
    firsts.clear();
    follows.clear();
    predictions.clear();
    setFormulas();
    setNonTerminals();
    setTerminals();
    setFirst(formulas, terminals, nonTerminals, firsts);
    setFollow(formulas, terminals, nonTerminals, firsts, follows);
    setPrediction(formulas, terminals, nonTerminals, firsts, follows, predictions);
    writeAllIntoFile();
}
