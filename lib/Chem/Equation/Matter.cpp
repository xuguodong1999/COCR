#include "Matter.h"
#include "BalanceUtil.h"
#include "XGD/Base/Util.h"
#include "XGD/Base/MaterialType.h"

#include <iomanip>
#include <unordered_set>
#include <unordered_map>

using namespace std;
const int ELECOL = 3;

Matter::Matter() {
    amount = 0;
    state = 'l';
    q = 0;
}

Matter::Matter(string &_name) {
    name = _name;
    amount = 0;
    state = 'l';
    q = 0;
}

Matter::Matter(char *_name) {
    name = _name;
    amount = 0;
    state = 'l';
    q = 0;
}

Matter::~Matter() {
    clear();
}

void Matter::clear() {
    name.clear();
    stateInfo.clear();
    eleList.clear();
    amount = 0;
    state = 'l';
    q = 0;
}

void Matter::inputName() {
    getline(std::cin, name);
}

bool Matter::Exist() {
    return MaterialUtil::Exist(name);
}


void Matter::getStateInfo() {
    const auto &info = MaterialUtil::QueryStateInfo(name);
    if (info.empty()) {
        state = 'g'; // init as gass
    } else {
        state = info[0];
    }
}

bool Matter::intoElement() {
    //  格式预处理
    string tar = name;
    if (!preProcess(tar)) {
#ifdef TSETCEI
        xgd::sout << "数据预处理失败！处理前为 \""
            << name << "\" ；处理后为 \"" << tar << "\" 。" << endl;
#endif
        return 0;
    }
    //  词法分析
    int i = 0, j = 0, k = 0;
    vector<MatNode> matlist;
    if (!lexicalAnalysis(matlist, tar)) {
#ifdef TESTCEI
        xgd::sout << "词法分析失败！处理前为 \"" << name << "\" ，已处理的信息：" << endl;
        xgd::sout << "(int)matlist.size=" << (int)matlist.size() << ", matlist=";
        for (i = 0; i < (int)matlist.size(); i++)
            matlist[i].print();
#endif
        return 0;
    }
    //  进一步词法分析，循环处理最小单元
    furtherLexicalAnalysis(matlist);
    //  存入元素列表
    eleList.clear();
    for (i = 0; i < (int) matlist[0].block.size(); i++)
        eleList.push_back(matlist[0].block[i]);
    matlist.clear();
#ifdef TESTCEI
    xgd::sout << "物质解析完成，M(" << name << ") = " << getRelativeMolecureMass() << " g/mol" << endl;
#endif
    return 1;
}

void Matter::printElement() {
    size_t i = 0;
    size_t col = 1;
    size_t leneleList = eleList.size();
    for (i = 0; i < leneleList; i++)
        col = (col > eleList[i].name.length() ? col : eleList[i].name.length());
    for (i = 0; i < leneleList; i++)
        xgd::sout << name << " 的元素 " << std::setw(col) << eleList[i].name << " 有 " << std::setw(4)
                  << eleList[i].amount
                  << "份" << endl;
}


bool Matter::getValence() {
    const auto &v = MaterialUtil::QueryValence(name);
    if (v.size() != eleList.size()) {
        return false;
    }
    for (size_t i = 0; i < eleList.size(); i++) {
        eleList[i].valence = v[i];
    }
    return true;
}

bool Matter::lexicalAnalysis(vector<MatNode> &matlist, string &tar) {
    Element eletmp;
    MatNode tmpnode;
    int tarlen = (int) tar.length();
    int i = 0, iend = 0, j = 0, amount = 0;
    for (i = 0; i < tarlen; i++) {
        tmpnode.clear();
        if (tar[i] >= '0' && tar[i] <= '9') {
            amount = 0;
            j = i;
            while (j < tarlen && tar[j] >= '0' && tar[j] <= '9')
                amount = amount * 10 + tar[j++] - '0';
            i = j - 1;
            tmpnode.num = amount;
            tmpnode.isNum = 1;
            if (i == 0 || (i > 0 && tar[i - 1] == '.'))
                tmpnode.isHead = 1;
            else
                tmpnode.isHead = 0;
        } else if ((tar[i] >= 'a' && tar[i] <= 'z') || (tar[i] >= 'A' && tar[i] <= 'Z')) {
            iend = ((i + ELECOL >= tarlen) ? (tarlen - 1) : (i + ELECOL));
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
#ifdef TESTCEI
                xgd::sout << "遇到无法解析的字符串！在 \"" << tar << "\" 中。" << endl;
#endif
                return 0;
            }
            eletmp.amount = 1;
            tmpnode.block.push_back(eletmp);
            tmpnode.isNum = 0;
            i = iend;
        } else {//  用num的负数域标记特殊字符
            switch (tar[i]) {
                case '(':
                    tmpnode.num = -7;
                    break;
                case ')':
                    tmpnode.num = -6;
                    break;
                default:
#ifdef TESTCEI
                    xgd::sout << "发现不合法字符！是 \"" << tar << "\" 中的第"
                        << i + 1 << "个字符 \"" << tar[i] << "\" 。" << endl;
#endif
                    return 0;
            }
            tmpnode.isNum = 1;
        }
        matlist.push_back(tmpnode);
    }
    return 1;
}

void Matter::furtherLexicalAnalysis(vector<MatNode> &matlist) {
    bool leaveloop = 1;
    int i = 0;
    int j = 0;
    int k = 0;
    while (1) {
        for (i = 0; i < (int) matlist.size(); i++)
            if (matlist[i].isNum == 1 && matlist[i].num < 0) {//  如果还存在括号
                leaveloop = 0;
                break;
            }
        if (leaveloop) break;
        leaveloop = 1;
        for (i = 0; i < (int) matlist.size(); i++) {//  追踪法得到一组可计算括号的位置(j, k)
            if (matlist[i].isNum == 1) {
                if (matlist[i].num == -7)
                    j = i;
                else if (matlist[i].num == -6) {
                    k = i;
                    break;
                }
            }
        }
        CalaBlock(matlist, j + 1, k - 1);
        matlist.erase(matlist.begin() + j + 1, matlist.begin() + k + 1);
    }
    CallastBlock(matlist);
}

bool Matter::CalaBlock(vector<MatNode> &matlist, int i, int j) {
    //  处理一个不含有括号的式子 [i, j]， 数据全部存入matlist[i - 1]
    int k = 0;
    int head = 1;
    int coef = 0;
    int is = i - 1;
    matlist[is].clear();
    if (matlist[i].isNum)                //  注意把前缀数字保留下来最后乘给整个元素整列 否则前缀为1 不影响结果
        head = matlist[i++].num;        //  只处理 matnode[is] 当然后缀只负责更新matnode[k]的数量
    //  开始循环处理 元素阵列的特征   A2 B C3 O ......， 处理完把数据扔到 matlist[i-1] 里面
    //  注意这里的A B C O 本身就是元素阵列 进行循环累加
    while (i <= j) {            //  预设两个matnode为一组 检查第二个是不是数字 否则一个matnode为一组
        coef = 1;
        if (i + 1 <= j && matlist[i + 1].isNum == 1)
            coef = matlist[i + 1].num;
        for (k = 0; k < (int) matlist[i].block.size(); k++) {    //  处理 matlist[i]
            matlist[i].block[k].amount *= (head * coef);        //  修正元素系数
            matlist[is].block.push_back(matlist[i].block[k]);    //  把这个Ele挂到阵列后面
        }
        i++;
        while (i <= j && matlist[i].isNum == 1)i++;                //  循环完成以后 matlist[i]是一个字母
    }
    matlist[is].compress();        //  处理完以后压缩一次元素阵列
    return 1;
}

bool Matter::CallastBlock(vector<MatNode> &matlist) {
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
        for (k = 0; k < (int) matlist[i].block.size(); k++) {
            matlist[i].block[k].amount *= (head * coef);
            tmp.block.push_back(matlist[i].block[k]);
        }
        i++;
        while (i <= j && matlist[i].isNum == 1)i++;
    }
    tmp.compress();
    matlist.clear();
    matlist.push_back(tmp);
    return 1;
}

bool Matter::preProcess(string &_tar) {
    //  去空格 统一分隔符样式
    auto tar = StdUtil::replaceSubStr(
            StdUtil::replaceSubStr(
                    StdUtil::replaceSubStr(
                            StdUtil::replaceSubStr(
                                    StdUtil::deleteSubStr(
                                            _tar, " "),
                                    "{", "("),
                            "[", "("),
                    "]", ")"),
            "}", ")");
    int j = 0;
    for (j = 0; j < (int) tar.size(); j++) {
        if (!((tar[j] >= '0' && tar[j] <= '9') || (tar[j] >= 'A' && tar[j] <= 'Z') ||
              (tar[j] >= 'a' && tar[j] <= 'z') || (tar[j] == '.') || (tar[j] == '(') || (tar[j] == ')') ||
              tar[j] == '\\' || tar[j] == '^'))
            return 0;
    }
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
    tar = StdUtil::deleteSubStr(tar, ".");
    //  去电荷标识符，规定，机内正电荷用\oplus，负电荷用\ominus表示，就近结合
    size_t pos = 0;
    string positive = "^\\oplus";
    string negative = "^\\ominus";
    q = 0;
    int cq = 0;
    int exp = 1;
    while (1) {
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
    q += cq;
    cq = 0;
    exp = 1;
    while (1) {
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
    q -= cq;
    _tar = StdUtil::deleteSubStr(StdUtil::deleteSubStr(tar, "^\\oplus"), "^\\ominus");
    return 1;
}

int Matter::getEleAmount(string &_element) {
    for (int i = 0; i < (int) eleList.size(); i++)
        if (_element == eleList[i].name)
            return eleList[i].amount;
    return 0;
}

int Matter::getEleIndex(string &_element) {
    for (int i = 0; i < (int) eleList.size(); i++)
        if (_element == eleList[i].name)
            return i;
    return -1;
}

string Matter::p() {
    string res = name + stateInfo;
    return res;
}

double Matter::getRelativeMolecureMass() {
    int i = 0;
    double res = 0;
    for (i = 0; i < (int) eleList.size(); i++)
        res += eleList[i].amount * Get_relative_atomic_mass(eleList[i].name);
    return res;
}
