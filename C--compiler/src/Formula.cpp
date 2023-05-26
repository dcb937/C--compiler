#include "Formula.h"
using namespace std;

Formula::Formula()
{
    // 默认构造函数的实现
}

Formula::Formula(string& left, vector<string>& right)
{
    this->left = left;
    this->right = right;
}

vector<string> Formula::returnRights()
{
    return right;
}

string Formula::returnLeft()
{
    return left;
}
