#include "equation.h"
#include "balance_util.h"
#include "base/std_util.h"
#include "math/linear_solver.h"
#include <algorithm>
using namespace std;

Equation::Equation() {
    isChargeContained = 0;
}

Equation::~Equation() {
    clear();
}

void Equation::clear() {
    isChargeContained = 0;
    redox.clear();
    mEquation.clear();
    mInput.clear();
    leftMatters.clear();
    rightMatters.clear();
    leftElements.clear();
    rightElements.clear();
    ratioList.clear();
    oRatioList.clear();
}

void Equation::inputContent() {
    getline(std::cin, mInput);
}

void Equation::getState() {
    for (int i = 0; i < (int) leftMatters.size(); i++)
        leftMatters[i].getStateInfo();
    for (int i = 0; i < (int) rightMatters.size(); i++)
        rightMatters[i].getStateInfo();
}

bool Equation::checkFormat() {
    mInput = StdUtil::deleteSubStr(mInput, " ");
    int len = (int) mInput.length();
    if (len < 3)
        return 0;
    if (mInput[0] == '=' || mInput[len - 1] == '=')
        return 0;
    int tmp = 0;
    for (int i = 1; i < len - 1; i++) {
        if (mInput[i] == '=') {
            tmp++;
            while (mInput[i] == '=')
                i++;
        }
    }
    if (tmp != 1)
        return 0;
    return 1;//确保中间有等号，是个方程
}

void Equation::split() {
    int i = 0;
    std::string ca;
    int index = 0;
    int len = (int) mInput.length();
    int start = 0;
    while (i < len) {
        if (mInput[i] < '0' || mInput[i] > '9') {
            oRatioList.push_back(1);
            while (i != len && mInput[i] != '+' && mInput[i] != '=')i++;
            i++;
        } else {
            start = i;
            ca.clear();
            ca.push_back(mInput[i++]);
            while (mInput[i] >= '0' && mInput[i] <= '9') {
                ca.push_back(mInput[i++]);
            }
            oRatioList.push_back(std::stoi(ca));
            index = ca.length();
            mInput.erase(start, index);
            len = (int) mInput.length();
            i = i - index;
            while (i != len && (mInput[i] != '+' && mInput[i] != '='))i++;
            i++;
        }
    }
    mEquation = mInput;
}

bool Equation::intoMatter() {
    //  检查方程格式
    if (!checkFormat()) {
#ifdef TESTCEI
        cout << "不满足方程的格式：XXX=...=XXX！" << endl;
#endif
        return 0;
    }
    if (mInput.find("^\\oplus") != string::npos || mInput.find("^\\ominus") != string::npos)
        isChargeContained = 1;
    else
        isChargeContained = 0;
    split();//  得到去系数方程
    //  处理去系数方程
    int len = (int) mEquation.length();
    int p = 0;
    Matter mat;
    bool is_reac = 1;
    while (p != len) {
        if (mEquation[p] == '+') {
            if (is_reac)
                leftMatters.push_back(mat);
            else
                rightMatters.push_back(mat);
            mat.clear();
        } else if (mEquation[p] == '=') {
            if (is_reac)
                leftMatters.push_back(mat);
            else
                rightMatters.push_back(mat);
            mat.clear();
            is_reac = 0;
            while (p != len - 1 && mEquation[p] == '=')p++;
            p--;
        } else if (p == len - 1) {
            mat.name += mEquation[p];
            if (is_reac)
                leftMatters.push_back(mat);
            else
                rightMatters.push_back(mat);
            mat.clear();
        } else
            mat.name += mEquation[p];
        p++;
    }
    //  元素级拆分
    if (!intoElement()) {
#ifdef TESTCEI
        cout << "字符串拆分失败！未完成元素级拆分。" << endl;
#endif
        return 0;
    }
    return 1;
}

void Equation::addStateInfo() {
    for (int i = 0; i < (int) leftMatters.size(); i++)
        leftMatters[i].getStateInfo();
    for (int i = 0; i < (int) rightMatters.size(); i++)
        rightMatters[i].getStateInfo();
    bool addUp = 1;
    bool addDown = 1;
    for (int i = 0; i < (int) leftMatters.size(); i++)
        if (leftMatters[i].state == 'g') {
            addUp = 0;
            break;
        }
    for (int i = 0; i < (int) leftMatters.size(); i++)
        if (leftMatters[i].state == 's' || leftMatters[i].state == 'm') {
            addDown = 0;
            break;
        }
    for (int i = 0; i < (int) rightMatters.size(); i++) {
        if (addUp && rightMatters[i].state == 'g')
            rightMatters[i].stateInfo = "↑";
        else if (addDown && rightMatters[i].state == 's')
            rightMatters[i].stateInfo = "↓";
    }
}

string Equation::p() {
//    char *tmp = new char[MAXC];
    std::string tmp = std::to_string(ratioList[0]);
    string res = "";
//    i2a(ratioList[0], tmp, 10);
    if (ratioList[0] != 1)res += tmp;
    res += add_html(leftMatters[0].name);
    int i = 0;
    int rs = (int) leftMatters.size();
    for (i = 1; i < rs; i++) {
        res += " + ";
        tmp = std::to_string(ratioList[i]);
//        i2a(ratioList[i], tmp, 10);
        if (ratioList[i] != 1)res += tmp;
        res += add_html(leftMatters[i].name);
    }
    res += " = ";
//    i2a(ratioList[rs], tmp, 10);
    tmp = std::to_string(ratioList[rs]);
    if (ratioList[rs] != 1)res += tmp;
    res = res + add_html(rightMatters[0].name) + rightMatters[0].stateInfo;
    for (i = 1; i < (int) rightMatters.size(); i++) {
        res += " + ";
        tmp = std::to_string(ratioList[i + rs]);
//        i2a(ratioList[i + rs], tmp, 10);
        if (ratioList[i + rs] != 1)res += tmp;
        res = res + add_html(rightMatters[i].name) + rightMatters[i].stateInfo;
    }
//    delete[]tmp;
    return res;
}

bool Equation::intoElement() {
    //拆分
    for (int i = 0; i < (int) leftMatters.size(); i++)
        if (!leftMatters[i].intoElement()) {
#ifdef TESTCEI
            cout << "字串 " << leftMatters[i].name << " 解析失败！" << endl;
#endif
            return 0;
        }
    for (int i = 0; i < (int) rightMatters.size(); i++)
        if (!rightMatters[i].intoElement()) {
#ifdef TESTCEI
            cout << "字串 " << rightMatters[i].name << " 解析失败！" << endl;
#endif
            return 0;
        }
    //  反应物元素累加、排序、合并
    for (int i = 0; i < (int) leftMatters.size(); i++) {
        for (int j = 0; j < (int) leftMatters[i].eleList.size(); j++)
            leftElements.push_back(leftMatters[i].eleList[j]);
    }
    std::sort(leftElements.begin(), leftElements.end(), namelessthan);
    for (int i = 0; i < (int) leftElements.size() - 1; i++) {
        int j = i + 1;
        while (j < (int) leftElements.size() && leftElements[i].name == leftElements[j].name) {
            leftElements[i].amount += leftElements[j].amount;
            j++;
        }
        leftElements.erase(leftElements.begin() + i + 1, leftElements.begin() + j);
    }
    //  生成物元素累加、排序、合并
    for (int i = 0; i < (int) rightMatters.size(); i++) {
        for (int j = 0; j < (int) rightMatters[i].eleList.size(); j++)
            rightElements.push_back(rightMatters[i].eleList[j]);
    }
    sort(rightElements.begin(), rightElements.end(), namelessthan);
    for (int i = 0; i < (int) rightElements.size() - 1; i++) {
        int j = i + 1;
        while (j < (int) rightElements.size() && rightElements[i].name == rightElements[j].name) {
            rightElements[i].amount += rightElements[j].amount;
            j++;
        }
        rightElements.erase(rightElements.begin() + i + 1, rightElements.begin() + j);
    }
    return 1;
}

bool Equation::balance() {
    if (leftElements.size() != rightElements.size()) {
        return false;
    }
    size_t col = getMattersNum() + 1;
    size_t row = leftElements.size();
    std::vector<std::vector<frac>> inMatrix(row, std::vector<frac>(col, 0));
    for (size_t i = 0; i < leftElements.size(); i++) {
        for (size_t j = 0; j < leftMatters.size(); j++)
            inMatrix[i][j] = leftMatters[j].getEleAmount(leftElements[i].name);
        for (size_t j = leftMatters.size(); j < leftMatters.size() + rightMatters.size(); j++)
            inMatrix[i][j] = -rightMatters[j - leftMatters.size()].getEleAmount(leftElements[i].name);
    }
    // 添加待定系数
    inMatrix.push_back(std::vector<frac>(col, 0));
    row += 1;
    inMatrix.back().at(0) = 1;
    inMatrix.back().back() = 1;
    if (isChargeContained && row < col - 1) {
        // 添加电荷守恒信息
        inMatrix.push_back(std::vector<frac>(col, 0));
        row += 1;
        for (size_t i = 0; i < leftMatters.size(); i++) {
            inMatrix.back().at(i) = leftMatters[i].q;
        }
        for (size_t i = 0; i < rightMatters.size(); i++) {
            inMatrix.back().at(i + leftMatters.size()) = -rightMatters[i].q;
        }
    }
    if (row < col - 1) {
        // 添加氧化还原信息
        inMatrix.push_back(std::vector<frac>(col, 0));
        row += 1;
        redox.clear();
        redox.resize(col, 0);
        if (!getRedox()) {
            return false;
        }
        for (size_t i = 0; i < redox.size(); i++) {
            inMatrix.back().at(i) = redox[i];
        }
    }
    if (row < col - 1) {
        // 添加随机系数
        inMatrix.push_back(std::vector<frac>(col, 0));
        row += 1;
        inMatrix.back().at(0) = 1;
        inMatrix.back().at(1) = 1;
    }
    if (row < col - 1) {
        return false;
    }
    auto outMatrix = linearSolve(inMatrix);
    if (outMatrix.size() != getMattersNum()) {
        // 添加随机系数
        inMatrix.push_back(std::vector<frac>(col, 0));
        row += 1;
        inMatrix.back().at(0) = 1;
        inMatrix.back().at(1) = 1;
        outMatrix = linearSolve(inMatrix);
        if (outMatrix.size() != getMattersNum()) {
            return false;
        }
    }
    ratioList.clear();
    for (size_t i = 0; i < outMatrix.size(); i++) {
        ratioList.push_back(outMatrix[i].numerator());
    }
    return true;
}

bool Equation::getRedox() {
    //获得化合价
    int cr = (int) leftMatters.size(), cp = (int) rightMatters.size();
    int i = 0, j = 0, i0 = 0, j0 = 0;
    for (i = 0; i < cr; i++)
        if (!leftMatters[i].getValence()) {
#ifdef TESTCEI
            cout << "无法获得 " << leftMatters[i].name << " 的化合价，配平失败！" << endl;
#endif
            return 0;
        }
    for (i = 0; i < cp; i++)
        if (!rightMatters[i].getValence()) {
#ifdef TESTCEI
            cout << "无法获得 " << rightMatters[i].name << " 的化合价，配平失败！" << endl;
#endif
            return 0;
        }
    for (i = 0; i < cr; i++) {            //  扫描所有反应物 扫描一个反应物的所有元素
        for (j = 0; j < (int) leftMatters[i].eleList.size(); j++) {
            for (i0 = 0; i0 < cp; i0++)    //  把该元素的化合价跟产物里面所有该元素的化合价进行比对
                for (j0 = 0; j0 < (int) rightMatters[i0].eleList.size(); j0++)
                    if (leftMatters[i].eleList[j].name == rightMatters[i0].eleList[j0].name &&
                        leftMatters[i].eleList[j].valence != rightMatters[i0].eleList[j0].valence) {
                        leftMatters[i].eleList[j].Reduced = 1;
                        goto BREAKPOINT1;//  第一轮遍历确定哪些元素化合价可能不一样
                    }
            BREAKPOINT1:;
        }
    }
    for (i = 0; i < cp; i++) {
        for (j = 0; j < (int) rightMatters[i].eleList.size(); j++) {
            for (i0 = 0; i0 < cr; i0++)
                for (j0 = 0; j0 < (int) leftMatters[i0].eleList.size(); j0++)
                    if (rightMatters[i].eleList[j].name == leftMatters[i0].eleList[j0].name &&
                        rightMatters[i].eleList[j].valence != leftMatters[i0].eleList[j0].valence) {
                        rightMatters[i].eleList[j].Reduced = 1;
                        goto BREAKPOINT2;
                    }
            BREAKPOINT2:;
        }
    }
    //  除掉同种元素中化合价没有变化的那部分
    for (i = 0; i < cr; i++) {
        for (j = 0; j < (int) leftMatters[i].eleList.size(); j++) {
            for (i0 = 0; i0 < cp; i0++)
                for (j0 = 0; j0 < (int) rightMatters[i0].eleList.size(); j0++)
                    if (leftMatters[i].eleList[j].name == rightMatters[i0].eleList[j0].name &&
                        leftMatters[i].eleList[j].valence == rightMatters[i0].eleList[j0].valence) {
                        leftMatters[i].eleList[j].Reduced = 0;
                        goto BREAKPOINT3;
                    }
            BREAKPOINT3:;
        }
    }
    for (i = 0; i < cp; i++) {
        for (j = 0; j < (int) rightMatters[i].eleList.size(); j++) {
            for (i0 = 0; i0 < cr; i0++)
                for (j0 = 0; j0 < (int) leftMatters[i0].eleList.size(); j0++)
                    if (rightMatters[i].eleList[j].name == leftMatters[i0].eleList[j0].name &&
                        rightMatters[i].eleList[j].valence == leftMatters[i0].eleList[j0].valence) {
                        rightMatters[i].eleList[j].Reduced = 0;
                        goto BREAKPOINT4;
                    }
            BREAKPOINT4:;
        }
    }
    //  确定氧化剂和还原剂
    for (i = 0; i < cr; i++) {
        for (j = 0; j < (int) leftMatters[i].eleList.size(); j++)
            for (i0 = 0; i0 < cp; i0++)
                for (j0 = 0; j0 < (int) rightMatters[i0].eleList.size(); j0++)
                    if (leftMatters[i].eleList[j].name == rightMatters[i0].eleList[j0].name &&
                        leftMatters[i].eleList[j].valence != rightMatters[i0].eleList[j0].valence &&
                        rightMatters[i0].eleList[j0].Reduced == 1)
                        if ((abs(leftMatters[i].eleList[j].finalValence - leftMatters[i].eleList[j].valence) >
                             abs(rightMatters[i0].eleList[j0].valence - leftMatters[i].eleList[j].valence)))
                            leftMatters[i].eleList[j].finalValence = rightMatters[i0].eleList[j0].valence;
    }
    //  写入数据
    frac tmp;
    for (i = 0; i < cr; i++) {
        for (j = 0; j < (int) leftMatters[i].eleList.size(); j++)
            if (leftMatters[i].eleList[j].Reduced) {//  如果该元素参与了反应
//                tmp.a = leftMatters[i].eleList[j].amount;
//                tmp.b = 1;
                tmp.set(leftMatters[i].eleList[j].amount, 1);
                redox[i] =
                        redox[i] + tmp * (leftMatters[i].eleList[j].valence - leftMatters[i].eleList[j].finalValence);
            }
    }
#ifdef TESTCEI
    cout << "氧化还原守恒向量：[";
    for (int i = 0; i < cr + cp; i++)
        printf("%4d", redox[i].intValue());
    cout << "      ]" << endl;
#endif
    return 1;
}

bool Equation::IsConversed(const vector<int> &raw) {
    int cer = (int) leftElements.size(), cep = (int) rightElements.size();
    int cr = (int) leftMatters.size(), cp = (int) rightMatters.size();
    if (cer != cep) {
        return 0;
    }
    int i = 0, j = 0, k = 0;
    for (i = 0; i < cer; i++) {
        if (leftElements[i].name != rightElements[i].name)
            return 0;
    }
//    int *check = new int[MAXM];
//    for (i = 0; i < MAXM; i++)check[i] = 0;
    std::vector<int> check(std::max(cer, cep), 0);
    for (i = 0; i < cr; i++) {
        for (j = 0; j < (int) leftMatters[i].eleList.size(); j++)
            check[getEleIndex(leftMatters[i].eleList[j].name)] += raw[i] * leftMatters[i].eleList[j].amount;
    }
    for (i = 0; i < cp; i++) {
        for (j = 0; j < (int) rightMatters[i].eleList.size(); j++)
            check[getEleIndex(rightMatters[i].eleList[j].name)] -= raw[i + cr] * rightMatters[i].eleList[j].amount;
    }
    for (i = 0; i < cer; i++) {
        if (check[i] != 0) {
//            delete[]check;
            return 0;
        }
    }
//    delete[] check;
    return 1;
}

int Equation::getEleIndex(string &tarelenam) {
    for (int i = 0; i < (int) leftElements.size(); i++)
        if (tarelenam == leftElements[i].name)
            return i;
    return -1;
}

bool Equation::exec(const string &_content) {
    mInput = _content;
    return exec();
}

bool Equation::exec() {
    if (!intoMatter()) {
#ifdef TESTCEI
        cout << "字符串拆分失败！未完成物质级拆分。" << endl;
#endif
        return 0;
    }
    if (!balance()) {
#ifdef TESTCEI
        cout << "方程式配平失败！" << endl;
#endif
        return 0;
    }
    addStateInfo();
    if (!IsConversed(ratioList)) {
#ifdef TESTCEI
        cout << "机器配平错误！" << endl;
#endif
        return 0;
    }
    return 1;
}

const size_t Equation::getMattersNum() const {
    return leftMatters.size() + rightMatters.size();
}

std::vector<int> Equation::getRatioList() const {
    return ratioList;
}
