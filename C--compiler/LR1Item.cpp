#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class LR1Item
{
public:
    std::string left;
    std::vector<std::string> right;
    int dot;           // 点的位置
    std::string ahead; // 展望

    // 构造函数
    LR1Item(std::string left, std::vector<std::string> right, int dot, std::string ahead)
        : left(left), right(right), dot(dot), ahead(ahead) {}

    // 输出字符串表示
    std::string toString()
    {
        return "[left=" + left + ", right=" + toString(right) + ", dot=" + std::to_string(dot) + ", ahead=" + ahead + "]";
    }

    // 判断是否相等
    bool equals(const LR1Item& other) const
    {
        return left == other.left && right == other.right && dot == other.dot && ahead == other.ahead;
    }
    bool operator==(const LR1Item& other) const
    {
        return left == other.left && right == other.right && dot == other.dot && ahead == other.ahead;
    }

    // 计算哈希值
    size_t hashCode() const
    {
        size_t leftHash = std::hash<std::string>{}(left);
        size_t aheadHash = std::hash<std::string>{}(ahead);
        return leftHash * 100 + aheadHash * 10 + dot;
    }

    // 闭包操作
    static std::unordered_set<LR1Item> closure(const std::unordered_set<LR1Item>& I, const SyntaxAnaly& analy);

    // 跳转操作
    static std::unordered_set<LR1Item> go(const std::unordered_set<LR1Item>& I, const std::string& X, const SyntaxAnaly& analy);

private:
    // // 辅助函数：将向量转换为字符串
    // std::string toString(const std::vector<std::string>& vec)
    // {
    //     std::string result = "[";
    //     for (size_t i = 0; i < vec.size(); ++i) {
    //         result += vec[i];
    //         if (i != vec.size() - 1) {
    //             result += ", ";
    //         }
    //     }
    //     result += "]";
    //     return result;
    // }
};

// 闭包操作
std::unordered_set<LR1Item> LR1Item::closure(const std::unordered_set<LR1Item>& I, const SyntaxAnaly& analy)
{
    std::unordered_set<std::string> vnSet = analy.vnSet;
    std::unordered_set<LR1Item> res = I;

    while (true) {
        std::unordered_set<LR1Item> newItems;
        size_t size = res.size();

        for (const auto& item : res) {
            if (item.dot < item.right.size() && vnSet.count(item.right[item.dot]) != 0) {
                std::vector<std::string> right0(item.right.begin() + item.dot + 1, item.right.end());
                right0.push_back(item.ahead);
                std::unordered_set<std::string> first = analy.getRightFirst(right0);

                const auto it = analy.express.find(item.right[item.dot]);
                if (it != analy.express.end()) {
                    const std::vector<std::vector<std::string>>& expressions = it->second;
                    for (const auto& right : expressions) {
                        for (const auto& vt : first) {
                            if (vt == analy.nullStr) {
                                continue;
                            }
                            LR1Item now(item.right[item.dot], right, 0, vt);
                            if (res.count(now) == 0 && newItems.count(now) == 0) {
                                newItems.insert(now);
                            }
                        }
                    }
                }
            }
        }

        res.insert(newItems);
        if (res.size() == size) {
            break;
        }
    }

    return res;
}

// 跳转操作
std::unordered_set<LR1Item> LR1Item::go(const std::unordered_set<LR1Item>& I, const std::string& X, const SyntaxAnaly& analy)
{
    std::unordered_set<LR1Item> J;

    for (const auto& item : I) {
        if (item.dot < item.right.size() && item.right[item.dot] == X) {
            LR1Item newItem(item.left, item.right, item.dot + 1, item.ahead);
            J.insert(newItem);
        }
    }

    return closure(J, analy);
}
