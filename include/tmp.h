#ifndef _TMP_H_
#define _TMP_H_
/**
 * 徐国栋大二写的代码好垃圾啊
 * 捏着鼻子重构中……
 */
#include "cocr.h"
#include "fraction.h"
#include "solver.h"
#include "chemdata.h"

#include<iostream>
#include<cstdlib>
#include<cmath>
#include<string>
#include<vector>
#include<algorithm>

using std::string;
using std::cout;
using std::endl;
using std::vector;
const int maxEleStrSize = 3;
using namespace cocr;

class Element {
public:
    Element() : amount(0), beginValence(0), isOxide(0), endValence(100) {

    }

    Element(string &name) : name(name) {
        Element();
    }

    string name;
    int amount;
    frac beginValence, endValence;

    void clear() {
        name.clear();
        amount = 0;
        beginValence = 0;
        isOxide = 0;
        endValence = 100;
    }

    bool Exist() {
        auto &cd = ChemistryData::GetInstance();
        return cd.isElementNameRecorded(name);
    }

    bool isOxide;
};

inline bool compElement(const Element &a, const Element &b) {
    return a.name < b.name;
}

class MatNode {
public:
    bool isNum, isHead;
    int num;
    //  content 代表元素阵列 记录一个matnode里面的所有元素
    vector<Element> content;

    MatNode() : num(0), isNum(false), isHead(false) {}

    void clear() {
        content.clear();
        num = 0;
        isNum = 0;
    }

    void compress() {    //  压缩元素阵列 合并相同的元素 累加元素数量
        sort(content.begin(), content.end(), compElement);
        int j = 0;
        for (int i = 0; i < (int) content.size() - 1; i++) {
            j = i + 1;
            while (j < (int) content.size() && content[i].name == content[j].name) {
                content[i].amount += content[j].amount;
                j++;
            }
            content.erase(content.begin() + i + 1, content.begin() + j);
        }
    }
};

class Matter {
public:
    void updateStateHint() {
        switch (commonState) {
            case ChemistryData::State::gass :
                stateHint = "↑";
                break;
            case ChemistryData::State::solid :
                stateHint = "↓";
                break;
            case ChemistryData::State::metal :
                stateHint = "↓";
                break;
            default:
                stateHint = "";
                break;
        }
    }

    Matter() : commonState(ChemistryData::State::unknown), charge(0) {
    }

    Matter(const string &name) : name(name) {
        Matter();
    }

    string name;
    int charge;
    ChemistryData::State commonState;
    string stateHint;

    void clear() {
        name.clear();
        stateHint.clear();
        elements.clear();
        commonState = ChemistryData::State::unknown;
        charge = 0;
    }

    void updateState() {
        auto &cd = ChemistryData::GetInstance();
        commonState = cd.getMatterState(name);
    }

    vector<Element> elements;

    ErrorCode toElements();

    bool updateValence();

    int getEleAmount(const string &element) {
        for (auto &ele:elements) {
            if (ele.name == element) {
                return ele.amount;
            }
        }
        return 0;
    }

    string toString() {
        return name + stateHint;
    }

private:
    ErrorCode preProcess(string &tar);

    bool procLastBlock(vector<MatNode> &matlist);

    bool procBlock(vector<MatNode> &matlist, int i, int j);

    ErrorCode toNodes(vector<MatNode> &matlist, string &tar);

    void procNodes(vector<MatNode> &matlist);
};

class Equation {
public:
    Equation();

    ~Equation();

    string content;

    void clear();

    void inputContent();

    bool IsConversed(vector<int> &raw);

    vector<int> redoxRatios;
    vector<int> ratios;

    bool exec(string &_content);

    bool exec();

private:
    string equation;
    bool hasChargeSym;
    vector<Matter> reactants;
    vector<Matter> products;
    vector<Element> elementL;
    vector<Element> elementR;
    vector<frac> redoxIndices;

    bool checkFormat();

    void removeRatio();

    ErrorCode intoMatter();

    ErrorCode intoElement();

    void updateElementState();

    void addStateInfo();

    ErrorCode balance();

    bool getRedox();

    int getEleIndex(string &tarelenam);
};


static map<string, string> convertMap = {
        {"·", "."},
        {"[", "("},
        {"{", "("},
        {"]", ")"},
        {"}", ")"}
};

inline string toHtml(string &tmpStr) {
    for (auto &i:convertMap) {
        replaceAllSubStr(tmpStr, i.first, i.second);
    }
    string res = "";
    int n = (int) tmpStr.length();
    for (int i = 0; i < tmpStr.length(); i++) {
        if (tmpStr[i] == '.') {
            res += "·";
            if (i + 1 < n && tmpStr[i + 1] >= '0' && tmpStr[i + 1] <= '9') {
                i++;
                while (tmpStr[i] >= '0' && tmpStr[i] <= '9')
                    res += tmpStr[i++];
                i--;
            }
        } else if (tmpStr[i] >= '0' && tmpStr[i] <= '9') {
            res += "<sub>";
            while (tmpStr[i] >= '0' && tmpStr[i] <= '9')
                res += tmpStr[i++];
            i--;
            res += "</sub>";
        } else
            res += tmpStr[i];
    }
    return res;
}


ErrorCode Matter::toElements() {
    //  格式预处理
    string tempName = name;
    if (ErrorCode::NormalExec != preProcess(tempName)) {
        return ErrorCode::CEPreProcessFailed;
    }
    //  词法分析
    int i = 0, j = 0, k = 0;
    vector<MatNode> nodes;
    if (ErrorCode::NormalExec != toNodes(nodes, tempName)) {
        return ErrorCode::CELexicalAnalysisFailed;
    }
    //  进一步词法分析，循环处理最小单元
    procNodes(nodes);
    //  存入元素列表
    elements = std::move(nodes[0].content);
    nodes.clear();
    return ErrorCode::NormalExec;
}


bool Matter::updateValence() {
    auto &cd = ChemistryData::GetInstance();
    auto val = cd.getMatterValence(name);
    if (val.size() != elements.size()) {
        return false;
    }
    for (auto i = 0; i < val.size(); i++) {
        elements[i].beginValence = val[i];
    }
    return true;
}

ErrorCode Matter::toNodes(vector<MatNode> &matlist, string &tar) {
    Element eletmp;
    MatNode tmpNode;
    int tarlen = (int) tar.length();
    int i = 0, iend = 0, j = 0, amount = 0;
    for (i = 0; i < tarlen; i++) {
        tmpNode.clear();
        if (tar[i] >= '0' && tar[i] <= '9') {
            amount = 0;
            j = i;
            while (j < tarlen && tar[j] >= '0' && tar[j] <= '9')
                amount = amount * 10 + tar[j++] - '0';
            i = j - 1;
            tmpNode.num = amount;
            tmpNode.isNum = 1;
            if (i == 0 || (i > 0 && tar[i - 1] == '.'))
                tmpNode.isHead = 1;
            else
                tmpNode.isHead = 0;
        } else if ((tar[i] >= 'a' && tar[i] <= 'z') || (tar[i] >= 'A' && tar[i] <= 'Z')) {
            iend = ((i + maxEleStrSize >= tarlen) ? (tarlen - 1) : (i + maxEleStrSize));
            while (iend >= i) {
                eletmp.clear();
                for (j = i; j <= iend; j++)
                    eletmp.name += tar[j];
                if (eletmp.Exist())
                    break;
                else
                    iend--;
            }
            if (iend < i) {
                return ErrorCode::CEUnknownLetter;
            }
            eletmp.amount = 1;
            tmpNode.content.emplace_back(eletmp);
            tmpNode.isNum = 0;
            i = iend;
        } else {//  用num的负数域标记特殊字符
            switch (tar[i]) {
                case '(':
                    tmpNode.num = -7;
                    break;
                case ')':
                    tmpNode.num = -6;
                    break;
                default:
                    return ErrorCode::CEUnknownChar;
            }
            tmpNode.isNum = 1;
        }
        matlist.push_back(tmpNode);
    }
    return ErrorCode::NormalExec;
}

void Matter::procNodes(vector<MatNode> &matlist) {
    bool breakOut = true;
    int i = 0;
    int j = 0;
    int k = 0;
    while (true) {
        for (i = 0; i < (int) matlist.size(); i++) {
            if (matlist[i].isNum == 1 && matlist[i].num < 0) {//  如果还存在括号
                breakOut = false;
                break;
            }
        }
        if (breakOut) break;
        breakOut = true;
        //  追踪法得到一组可计算括号的位置(j, k)
        for (i = 0; i < (int) matlist.size(); i++) {
            if (matlist[i].isNum == 1) {
                if (matlist[i].num == -7)
                    j = i;
                else if (matlist[i].num == -6) {
                    k = i;
                    break;
                }
            }
        }
        procBlock(matlist, j + 1, k - 1);
        matlist.erase(matlist.begin() + j + 1, matlist.begin() + k + 1);
    }
    procLastBlock(matlist);
}

bool Matter::procBlock(vector<MatNode> &matlist, int i, int j) {
    //  处理一个不含有括号的式子 [i, j]， 数据全部存入matlist[i - 1]
    int k = 0;
    int head = 1;
    int coef = 0;
    int is = i - 1;
    matlist[is].clear();
    //  注意把前缀数字保留下来最后乘给整个元素整列 否则前缀为1 不影响结果
    if (matlist[i].isNum)
        //  只处理 matnode[is] 当然后缀只负责更新matnode[k]的数量
        head = matlist[i++].num;
    //  开始循环处理 元素阵列的特征   A2 B C3 O ......，
    //  处理完把数据扔到 matlist[i-1] 里面
    //  注意这里的A B C O 本身就是元素阵列 进行循环累加
    //  预设两个matnode为一组 检查第二个是不是数字 否则一个matnode为一组
    while (i <= j) {
        coef = 1;
        if (i + 1 <= j && matlist[i + 1].isNum == 1)
            coef = matlist[i + 1].num;
        //  处理 matlist[i]
        for (k = 0; k < (int) matlist[i].content.size(); k++) {
            matlist[i].content[k].amount *= (head * coef);        //  修正元素系数
            //  把这个Ele挂到阵列后面
            matlist[is].content.emplace_back(matlist[i].content[k]);
        }
        i++;
        //  循环完成以后 matlist[i]是一个字母
        while (i <= j && matlist[i].isNum == 1)i++;
    }
    matlist[is].compress();        //  处理完以后压缩一次元素阵列
    return 1;
}

bool Matter::procLastBlock(vector<MatNode> &matlist) {
    int i = 0;
    int j = (int) matlist.size() - 1;
    int k = 0;
    int head = 1;
    int coef = 0;
    MatNode tmp;
    if (matlist[i].isNum)
        head = matlist[i++].num;
    while (i <= j) {
        coef = 1;
        if (i + 1 <= j && matlist[i + 1].isNum == 1)
            coef = matlist[i + 1].num;
        for (k = 0; k < (int) matlist[i].content.size(); k++) {
            matlist[i].content[k].amount *= (head * coef);
            tmp.content.emplace_back(matlist[i].content[k]);
        }
        i++;
        while (i <= j && matlist[i].isNum == 1)i++;
    }
    tmp.compress();
    matlist.clear();
    matlist.emplace_back(tmp);
    return true;
}

bool isValidChar(char a) {
    static const string validString = "0123456789qazwsxedcrfvtgbyhnujmikolpQAZWSXEDCRFVTGBYHNUJMIKOLP.()@$";
    return validString.find(a) != string::npos;
}

ErrorCode Matter::preProcess(string &tar) {
    //  去空格 统一分隔符样式
    removeAllSubStr(tar, " ");
    for (auto &i:convertMap) {
        replaceAllSubStr(tar, i.first, i.second);
    }
    for (auto c:tar) {
        if (!isValidChar(c))return ErrorCode::CEUnknownChar;
    }
    int j = 0;
    //  修饰并列符“.”
    for (int i = 0; i < (int) tar.length() - 1; i++) {
        if (tar[i] == '.') {
            tar.insert(tar.begin() + i + 1, '(');
            for (j = i + 1; j < (int) tar.length(); j++) {
                if (tar[j] == '.') {
                    tar.insert(tar.begin() + j, ')');
                    break;
                }
            }
            if (j == tar.length())tar.push_back(')');
        }
    }
    removeAllSubStr(tar, ".");
    //  去电荷标识符，规定，机内正电荷用@，负电荷用$表示，就近结合
    size_t pos = 0;
    string positive = "@";
    string negative = "$";
    charge = 0;
    int cq = 0;
    int exp = 1;
    while (true) {
        pos = tar.find(positive);
        if (pos == string::npos)break;
        tar.erase(pos, positive.size());
        pos--;
        while (pos >= 0 && tar[pos] >= '0' && tar[pos] <= '9') {
            cq = cq + exp * (tar[pos] - '0');
            exp *= 10;
            tar.erase(pos--, 1);
        }
        if (cq == 0)cq = 1;
    }
    charge += cq;
    cq = 0;
    exp = 1;
    while (true) {
        pos = tar.find(negative);
        if (pos == string::npos)break;
        tar.erase(pos, negative.size());
        pos--;
        while (pos >= 0 && tar[pos] >= '0' && tar[pos] <= '9') {
            cq = cq + exp * (tar[pos] - '0');
            exp *= 10;
            tar.erase(pos--, 1);
        }
        if (cq == 0)cq = 1;
    }
    charge -= cq;
    removeAllSubStr(tar, "@");
    removeAllSubStr(tar, "$");
    return ErrorCode::NormalExec;
}


Equation::Equation() {
    hasChargeSym = 0;
}

Equation::~Equation() {
    clear();
}

void Equation::clear() {
    hasChargeSym = 0;
    redoxIndices.clear();
    equation.clear();
    content.clear();
    reactants.clear();
    products.clear();
    elementL.clear();
    elementR.clear();
    ratios.clear();
    redoxRatios.clear();
}

void Equation::inputContent() {
    getline(std::cin, content);
}

void Equation::updateElementState() {
    for (int i = 0; i < (int) reactants.size(); i++)
        reactants[i].updateState();
    for (int i = 0; i < (int) products.size(); i++)
        products[i].updateState();
}

bool Equation::checkFormat() {
    removeAllSubStr(content, " ");
    int len = (int) content.length();
    if (len < 3)
        return 0;
    if (content[0] == '=' || content[len - 1] == '=')
        return 0;
    int tmp = 0;
    for (int i = 1; i < len - 1; i++) {
        if (content[i] == '=') {
            tmp++;
            while (content[i] == '=')
                i++;
        }
    }
    if (tmp != 1)
        return 0;
    return 1;//确保中间有等号，是个方程
}

void Equation::removeRatio() {
    int i = 0;
    string numStr;
    int index = 0;
    int len = (int) content.length();
    int start = 0;
    while (i < len) {
        if (!std::isdigit(content[i])) {
            redoxRatios.emplace_back(1);
            while (i != len && content[i] != '+' && content[i] != '=')i++;
            i++;
        } else {
            start = i;
            index = 0;
            numStr.clear();
            numStr.push_back(content[i++]);
            while (std::isdigit(content[i]))
                numStr.push_back(content[i++]);
            redoxRatios.emplace_back(std::stoi(numStr));
            content.erase(start, index);
            len = (int) content.length();
            i = i - index;
            while (i != len && content[i] != '+' && content[i] != '=')i++;
            i++;
        }
    }
    equation = content;
}

ErrorCode Equation::intoMatter() {
    //  检查方程格式
    if (!checkFormat()) {
        return ErrorCode::CEFormatError;
    }
    if (content.find("@") != string::npos ||
        content.find("$") != string::npos)
        hasChargeSym = true;
    else
        hasChargeSym = false;
    removeRatio();//  得到去系数方程
    //  处理去系数方程
    auto len = equation.length();
    int p = 0;
    Matter mat;
    bool is_reac = 1;
    while (p != len) {
        if (equation[p] == '+') {
            if (is_reac)
                reactants.emplace_back(mat);
            else
                products.emplace_back(mat);
            mat.clear();
        } else if (equation[p] == '=') {
            if (is_reac)
                reactants.emplace_back(mat);
            else
                products.emplace_back(mat);
            mat.clear();
            is_reac = 0;
            while (p != len - 1 && equation[p] == '=')p++;
            p--;
        } else if (p == len - 1) {
            mat.name += equation[p];
            if (is_reac)
                reactants.emplace_back(mat);
            else
                products.emplace_back(mat);
            mat.clear();
        } else
            mat.name += equation[p];
        p++;
    }
    //  元素级拆分
    if (ErrorCode::NormalExec != intoElement()) {
        return ErrorCode::CEDivElementFailed;
    }
    return ErrorCode::NormalExec;
}


void Equation::addStateInfo() {
    for (int i = 0; i < (int) reactants.size(); i++)
        reactants[i].updateState();
    for (int i = 0; i < (int) products.size(); i++)
        products[i].updateState();
    bool addUp = 1;
    bool addDown = 1;
    for (int i = 0; i < (int) reactants.size(); i++)
        if (reactants[i].commonState == cocr::ChemistryData::State::gass) {
            addUp = 0;
            break;
        }
    for (int i = 0; i < (int) reactants.size(); i++)
        if (reactants[i].commonState == cocr::ChemistryData::State::solid ||
            reactants[i].commonState == cocr::ChemistryData::State::metal) {
            addDown = 0;
            break;
        }
    for (int i = 0; i < (int) products.size(); i++) {
        if (addUp && products[i].commonState == cocr::ChemistryData::State::gass)
            products[i].stateHint = "↑";
        else if (addDown && products[i].commonState == cocr::ChemistryData::State::solid)
            products[i].stateHint = "↓";
    }
}


ErrorCode Equation::intoElement() {
    //拆分
    for (int i = 0; i < (int) reactants.size(); i++)
        if (ErrorCode::NormalExec != reactants[i].toElements()) {
            return ErrorCode::CEDivElementFailed;
        }
    for (int i = 0; i < (int) products.size(); i++)
        if (ErrorCode::NormalExec != products[i].toElements()) {
            return ErrorCode::CEDivElementFailed;
        }
    //  反应物元素累加、排序、合并
    for (int i = 0; i < (int) reactants.size(); i++) {
        for (int j = 0; j < (int) reactants[i].elements.size(); j++)
            elementL.emplace_back(reactants[i].elements[j]);
    }
    sort(elementL.begin(), elementL.end(), compElement);
    for (int i = 0; i < (int) elementL.size() - 1; i++) {
        int j = i + 1;
        while (j < (int) elementL.size() && elementL[i].name == elementL[j].name) {
            elementL[i].amount += elementL[j].amount;
            j++;
        }
        elementL.erase(elementL.begin() + i + 1, elementL.begin() + j);
    }
    //  生成物元素累加、排序、合并
    for (int i = 0; i < (int) products.size(); i++) {
        for (int j = 0; j < (int) products[i].elements.size(); j++)
            elementR.emplace_back(products[i].elements[j]);
    }
    sort(elementR.begin(), elementR.end(), compElement);
    for (int i = 0; i < (int) elementR.size() - 1; i++) {
        int j = i + 1;
        while (j < (int) elementR.size() && elementR[i].name == elementR[j].name) {
            elementR[i].amount += elementR[j].amount;
            j++;
        }
        elementR.erase(elementR.begin() + i + 1, elementR.begin() + j);
    }
    return ErrorCode::NormalExec;
}

ErrorCode Equation::balance() {
    int row = (int) elementL.size();
    if (row != (int) elementR.size()) {
        return ErrorCode::CEElementFailed;
    }
    int i = 0, j = 0, cr = (int) reactants.size();
    int col = cr + (int) products.size();
    vector<int> a(row * col);
    for (i = 0; i < row; i++) {
        for (j = 0; j < cr; j++)
            a[col * i + j] = reactants[j].getEleAmount(elementL[i].name);
        for (j = cr; j < col; j++)
            a[col * i + j] = -products[j - cr].getEleAmount(elementL[i].name);
    }

    vector<int> resint(col);
    vector<vector<frac>> augMatrix(row);
    vector<frac> result;
    for (auto &r:augMatrix) { r.resize(col); }
    int index_a = 0;
    for (i = 1; i <= row; i++) {
        for (j = 1; j <= col; j++)
            augMatrix[i][j] = a[index_a++];
    }
    auto &solver = LinearSolver::getInstance();
    bool success = solver.solve(augMatrix, result);
    if (success) {
        ratios.clear();
        for (j = 1; j <= col; j++)
            ratios.emplace_back(resint[j]);
    } else {// 尝试解第二次，加化合价
        augMatrix.resize(row + 1);
        augMatrix[row].resize(col);
        success = solver.solve(augMatrix, result);
        if (success) {

        } else {// 尝试解第三次，随机指定一组比例

        }
    }
    return ErrorCode::NormalExec;
}

bool Equation::getRedox() {
    //获得化合价
    int cr = (int) reactants.size(), cp = (int) products.size();
    int i = 0, j = 0, i0 = 0, j0 = 0;
    for (i = 0; i < cr; i++)
        if (!reactants[i].updateValence()) {
#ifdef TESTCEI
            cout << "无法获得 " << reactants[i].name << " 的化合价，配平失败！" << endl;
#endif
            return 0;
        }
    for (i = 0; i < cp; i++)
        if (!products[i].updateValence()) {
#ifdef TESTCEI
            cout << "无法获得 " << products[i].name << " 的化合价，配平失败！" << endl;
#endif
            return 0;
        }
    for (i = 0; i < cr; i++) {            //  扫描所有反应物 扫描一个反应物的所有元素
        for (j = 0; j < (int) reactants[i].elements.size(); j++) {
            for (i0 = 0; i0 < cp; i0++)    //  把该元素的化合价跟产物里面所有该元素的化合价进行比对
                for (j0 = 0; j0 < (int) products[i0].elements.size(); j0++)
                    if (reactants[i].elements[j].name == products[i0].elements[j0].name &&
                        reactants[i].elements[j].beginValence != products[i0].elements[j0].beginValence) {
                        reactants[i].elements[j].isOxide = 1;
                        goto BREAKPOINT1;//  第一轮遍历确定哪些元素化合价可能不一样
                    }
            BREAKPOINT1:;
        }
    }
    for (i = 0; i < cp; i++) {
        for (j = 0; j < (int) products[i].elements.size(); j++) {
            for (i0 = 0; i0 < cr; i0++)
                for (j0 = 0; j0 < (int) reactants[i0].elements.size(); j0++)
                    if (products[i].elements[j].name == reactants[i0].elements[j0].name &&
                        products[i].elements[j].beginValence != reactants[i0].elements[j0].beginValence) {
                        products[i].elements[j].isOxide = 1;
                        goto BREAKPOINT2;
                    }
            BREAKPOINT2:;
        }
    }

    //  除掉同种元素中化合价没有变化的那部分
    for (i = 0; i < cr; i++) {
        for (j = 0; j < (int) reactants[i].elements.size(); j++) {
            for (i0 = 0; i0 < cp; i0++)
                for (j0 = 0; j0 < (int) products[i0].elements.size(); j0++)
                    if (reactants[i].elements[j].name == products[i0].elements[j0].name &&
                        reactants[i].elements[j].beginValence == products[i0].elements[j0].beginValence) {
                        reactants[i].elements[j].isOxide = 0;
                        goto BREAKPOINT3;
                    }
            BREAKPOINT3:;
        }
    }
    for (i = 0; i < cp; i++) {
        for (j = 0; j < (int) products[i].elements.size(); j++) {
            for (i0 = 0; i0 < cr; i0++)
                for (j0 = 0; j0 < (int) reactants[i0].elements.size(); j0++)
                    if (products[i].elements[j].name == reactants[i0].elements[j0].name &&
                        products[i].elements[j].beginValence == reactants[i0].elements[j0].beginValence) {
                        products[i].elements[j].isOxide = 0;
                        goto BREAKPOINT4;
                    }

            BREAKPOINT4:;
        }
    }

    //  确定氧化剂和还原剂
    for (i = 0; i < cr; i++) {
        for (j = 0; j < (int) reactants[i].elements.size(); j++)
            for (i0 = 0; i0 < cp; i0++)
                for (j0 = 0; j0 < (int) products[i0].elements.size(); j0++)
                    if (reactants[i].elements[j].name == products[i0].elements[j0].name &&
                        reactants[i].elements[j].beginValence != products[i0].elements[j0].beginValence &&
                        products[i0].elements[j0].isOxide == 1)
                        if ((cocr::abs(reactants[i].elements[j].endValence - reactants[i].elements[j].beginValence) >
                             cocr::abs(products[i0].elements[j0].beginValence - reactants[i].elements[j].beginValence)))
                            reactants[i].elements[j].endValence = products[i0].elements[j0].beginValence;
    }
    //  写入数据
    frac tmp;
    for (i = 0; i < cr; i++) {
        for (j = 0; j < (int) reactants[i].elements.size(); j++)
            if (reactants[i].elements[j].isOxide) {//  如果该元素参与了反应
                tmp = reactants[i].elements[j].amount;
                redoxIndices[i] = redoxIndices[i] +
                                  tmp * (reactants[i].elements[j].beginValence - reactants[i].elements[j].endValence);
            }
    }
    return 1;
}

bool Equation::IsConversed(vector<int> &raw) {
    int eleLeftSize = elementL.size(), eleRightSize = elementR.size();
    int reactantSize = reactants.size(), productSize = products.size();
    if (eleLeftSize != eleRightSize) {
        return false;
    }
    for (auto i = 0; i < eleLeftSize; i++) {
        if (elementL[i].name != elementR[i].name)
            return false;
    }
    vector<int> check(eleLeftSize, 0);
    for (auto i = 0; i < reactantSize; i++) {
        for (auto j = 0; j < reactants[i].elements.size(); j++)
            check[getEleIndex(reactants[i].elements[j].name)] += raw[i] * reactants[i].elements[j].amount;
    }
    for (auto i = 0; i < productSize; i++) {
        for (auto j = 0; j < products[i].elements.size(); j++)
            check[getEleIndex(products[i].elements[j].name)] -= raw[i + reactantSize] * products[i].elements[j].amount;
    }
    for (auto i = 0; i < eleLeftSize; i++) {
        if (check[i] != 0) {
            return false;
        }
    }
    return true;
}

int Equation::getEleIndex(string &tarelenam) {
    for (int i = 0; i < (int) elementL.size(); i++)
        if (tarelenam == elementL[i].name)
            return i;
    return -1;
}

bool Equation::exec(string &_content) {
    content = _content;
    if (ErrorCode::NormalExec != intoMatter()) {
        return false;
    }
    if (ErrorCode::NormalExec != balance()) {
        return false;
    }
    addStateInfo();
    if (!IsConversed(ratios)) {
        return false;
    }
//    cout << p() << endl;
    clear();
    return true;
}


bool Equation::exec() {
    if (ErrorCode::NormalExec != intoMatter()) {
        return false;
    }
    if (ErrorCode::NormalExec != balance()) {
        return false;
    }
    addStateInfo();
    if (!IsConversed(ratios)) {
        return false;
    }
    return true;
}


#endif //_TMP_H_
