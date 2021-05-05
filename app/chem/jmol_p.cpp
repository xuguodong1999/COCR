#include "jmol_p.hpp"
#include "jatom.hpp"
#include <QDebug>

using namespace xgd;
using atom_t = std::shared_ptr<JAtom>;

std::unordered_map<std::string, TokenType> xgd::SUPER_ATOM_MAP;

size_t xgd::MAX_SUPER_ATOM_LENGTH = 0;

std::unordered_set<TokenType> TOKEN_BOND_SET = {
        TokenType::Single, TokenType::Double, TokenType::Triple
}, TOKEN_NEG_SET = {
        TokenType::Pos, TokenType::Neg
}, TOKEN_ABB_SET = {
        TokenType::Me, TokenType::Et, TokenType::iPr, TokenType::nPr, TokenType::iBu, TokenType::tBu,
        TokenType::nBu, TokenType::Am, TokenType::NO2, TokenType::NO, TokenType::SO3, TokenType::SO2,
        TokenType::SO, TokenType::CO2, TokenType::CO, TokenType::Ph, TokenType::Bz, TokenType::CSO, TokenType::COS,
        TokenType::N2H4, TokenType::N2H5, TokenType::Cbz, TokenType::SCN, TokenType::CN, TokenType::Bn, TokenType::Cbz,
//        // TODO
//         TokenType::OCN, TokenType::NCS,
//        TokenType::Ace, TokenType::THPO, TokenType::NHZ, TokenType::Ms
};

inline bool isChargeToken(const xgd::TokenType &_abb) {
    return TOKEN_NEG_SET.end() != TOKEN_NEG_SET.find(_abb);
}

inline bool isAbbToken(const xgd::TokenType &_abb) {
    return TOKEN_ABB_SET.end() != TOKEN_ABB_SET.find(_abb);
}

inline bool isBondToken(const xgd::TokenType &_abb) {
    return TOKEN_BOND_SET.end() != TOKEN_BOND_SET.find(_abb);
}

inline bool isNumberToken(const xgd::TokenType &_abb) {
    return TokenType::Number == _abb;
}

inline bool isElementToken(const xgd::TokenType &_abb) {
    return TokenType::Element == _abb;
}

inline bool isNoneToken(const xgd::TokenType &_abb) {
    return TokenType::None == _abb;
}

inline bool isLeftToken(const xgd::TokenType &_abb) {
    return TokenType::Left == _abb;
}

inline bool isRightToken(const xgd::TokenType &_abb) {
    return TokenType::Right == _abb;
}

void JMol_p::exceedValence() {
    isValenceDataLatest = false;
    atomTotalBondOrderMap.clear();
    atomTotalBondOrderMap.clear();
}

void JMol_p::confirmValence() {
    isValenceDataLatest = true;
}

void JMol_p::addBondOrder4Atom(const id_type &_aid, const int &_order) {
    auto it = atomTotalBondOrderMap.find(_aid);
    if (atomTotalBondOrderMap.end() == it) {
        atomTotalBondOrderMap[_aid] = _order;
    } else {
        it->second += _order;
    }
}

void JMol_p::addDoubleBondNum4Atom(const id_type &_aid, const int &_num) {
    auto it = atomDoubleBondNum.find(_aid);
    if (atomDoubleBondNum.end() == it) {
        atomDoubleBondNum[_aid] = 1;
    } else {
        it->second += _num;
    }
}

int JMol_p::getDoubleBondNum(const id_type &_aid) const {
    auto it = atomDoubleBondNum.find(_aid);
    if (atomDoubleBondNum.end() == it) { return 0; }
    return it->second;
}

int JMol_p::getAtomOrder(const id_type &_aid) {
    auto it = atomTotalBondOrderMap.find(_aid);
    if (atomTotalBondOrderMap.end() == it) {
        atomTotalBondOrderMap[_aid] = 0;
        return 0;
    } else {
        return it->second;
    }
}

void JMol_p::updateValenceMap() {
    atomTotalBondOrderMap.clear();
    atomDoubleBondNum.clear();
    // 初始化原子邻接键的键级累加表
    mol.loopBondVec([&](JBond &_bond) {
        int order = _bond.getBondOrder();
        auto from = _bond.getFrom();
        auto to = _bond.getTo();
        if (from && to) {
            addBondOrder4Atom(from->getId(), order);
            addBondOrder4Atom(to->getId(), order);
        }
        if (order == 2) {
            addDoubleBondNum4Atom(from->getId(), 1);
            addDoubleBondNum4Atom(to->getId(), 1);
        }
    });
    isValenceDataLatest = true;
}

bool JMol_p::IsValenceDataLatest() const {
    return isValenceDataLatest;
}

JMol_p::JMol_p(JMol &_mol) : mol(_mol), isValenceDataLatest(false), last_holder(nullptr) {

}

std::pair<atom_t, atom_t> JMol_p::makeAbbType(const TokenType &_abb) {
//    qDebug() << __FUNCTION__ << static_cast<int>(_abb);
    switch (_abb) {
        case TokenType::Me: {
            return makeAlkane(1);
        }
        case TokenType::Et: {
            auto[first, last]= makeAlkane(2);
            return {first, last};
        }
        case TokenType::iPr: {
            auto[first, last]=makeAlkane(2);
            mol.addBond(first, mol.addAtom(ElementType::C));
            return {first, first};
        }
        case TokenType::nPr: {
            auto[first, last]= makeAlkane(3);
            return {first, first};
        }
        case TokenType::iBu: {
            auto[first, last]=makeAlkane(3);
            mol.addBond(first, mol.addAtom(ElementType::C));
            return {first, first};
        }
        case TokenType::tBu: {
            auto[first, last]=makeAlkane(2);
            mol.addBond(first, mol.addAtom(ElementType::C));
            mol.addBond(first, mol.addAtom(ElementType::C));
            return {first, first};
        }
        case TokenType::nBu: {
            auto[first, last]= makeAlkane(4);
            return {first, first};
        }
        case TokenType::Am: {
            auto[first, last]= makeAlkane(5);
            return {first, first};
        }
        case TokenType::NO2: {
            auto[n, _]= makeAcyl(ElementType::O, ElementType::N);
            auto o1 = mol.addAtom(ElementType::O);
            mol.addBond(n, o1, BondType::SingleBond);
            return {n, n};
        }
        case TokenType::NO: {
            return makeAcyl(ElementType::O, ElementType::N);
        }
        case TokenType::SO3: {
            auto[s, _]= makeAcyl(ElementType::O, ElementType::S);
            auto o1 = mol.addAtom(ElementType::O);
            mol.addBond(s, o1, BondType::DoubleBond);
            auto o2 = mol.addAtom(ElementType::O);
            mol.addBond(s, o2);
            return {s, o2};
        }
        case TokenType::SO2: {
            auto[s, _]= makeAcyl(ElementType::O, ElementType::S);
            auto o = mol.addAtom(ElementType::O);
            mol.addBond(s, o, BondType::DoubleBond);
            return {s, s};
        }
        case TokenType::SO: {
            return makeAcyl(ElementType::O, ElementType::S);
        }
        case TokenType::CO2: {
            auto[c, _]= makeAcyl(ElementType::O, ElementType::C);
            auto o = mol.addAtom(ElementType::O);
            mol.addBond(c, o);
            return {c, o};
        }
        case TokenType::CO: {
            return makeAcyl(ElementType::O, ElementType::C);
        }
        case TokenType::Ph: {
            auto c6 = makeBenzene();
            auto c = std::get<0>(c6);
            return {c, c};
        }
        case TokenType::Bn: {
            auto C = mol.addAtom(ElementType::C);
            auto c6 = makeBenzene();
            auto c = std::get<0>(c6);
            mol.addBond(c, C);
            return {C, C};
        }
        case TokenType::Bz: {
            auto[co, _]= makeAcyl();
            auto c6 = makeBenzene();
            auto c = std::get<0>(c6);
            mol.addBond(c, co);
            return {co, co};
        }
        case TokenType::CSO: {
            auto[c, _]= makeAcyl(ElementType::S, ElementType::C);
            auto o = mol.addAtom(ElementType::O);
            mol.addBond(c, o);
            return {c, o};
        }
        case TokenType::COS: {
            auto[c, _]= makeAcyl(ElementType::O, ElementType::C);
            auto s = mol.addAtom(ElementType::O);
            mol.addBond(c, s);
            return {c, s};
        }
        case TokenType::N2H4: {
            auto[n1, n2]=makeAlkane(2, ElementType::N);
            return {n1, n2};
        }
        case TokenType::N2H5: {
            auto[n1, _]=makeAlkane(2, ElementType::N);
            return {n1, n1};
        }
        case TokenType::Cbz: {// 苄氧羰基
            auto[co, _]= makeAcyl();
            auto c6 = makeBenzene();
            auto c = std::get<0>(c6);
            auto o = mol.addAtom(ElementType::O);
            mol.addBond(c, o);
            mol.addBond(o, co);
            return {co, co};
        }
        case TokenType::SCN: {
            auto[s, _]=makeElementType(ElementType::S);
//            auto[s, _]=makeAlkane(1,ElementType::S);
            auto c = mol.addAtom(ElementType::C);
            auto n = mol.addAtom(ElementType::N);
            mol.addBond(s, c);
            mol.addBond(c, n, BondType::TripleBond);
            return {s, s};
        }
        case TokenType::CN: {
            auto[c, _]=makeElementType(ElementType::C);
//            auto[c, _]=makeAlkane(1,ElementType::C);
            auto n = mol.addAtom(ElementType::N);
            mol.addBond(c, n, BondType::TripleBond);
            return {c, c};
        }
        default: {
            break;
        }
    }
    return {nullptr, nullptr};
}

std::pair<atom_t, atom_t> JMol_p::makeElementType(const ElementType &_ele) {
    atom_t first;
    if (last_holder) {
        last_holder->setCharge(0);
        last_holder->setType(_ele);
        first = last_holder;
        clearLastHolder();
    } else {
        first = mol.addAtom(_ele);
    }
    return {first, first};
}

std::pair<atom_t, atom_t> JMol_p::makeAlkane(const int &_num, const ElementType &_ele) {
    if (_num <= 0) { return {nullptr, nullptr}; }
    atom_t first;
    if (last_holder) {
        last_holder->setCharge(0);
        last_holder->setType(_ele);
        first = last_holder;
        clearLastHolder();
    } else {
        first = mol.addAtom(_ele);
    }
    auto last = first;
    for (int i = 1; i < _num; i++) {
        auto next = mol.addAtom(_ele);
        mol.addBond(last, next);
        last = next;
    }
    return {first, last};
}

std::tuple<atom_t, atom_t, atom_t, atom_t, atom_t, atom_t> JMol_p::makeBenzene() {
    atom_t atoms[6];
    if (last_holder) {
        last_holder->setCharge(0);
        last_holder->setType(ElementType::C);
        atoms[0] = last_holder;
        clearLastHolder();
    } else {
        atoms[0] = mol.addAtom(ElementType::C);
    }
    for (int i = 1; i < 6; i++) {
        atoms[i] = mol.addAtom(ElementType::C);
        mol.addBond(atoms[i], atoms[i - 1], i % 2 ? BondType::SingleBond : BondType::DoubleBond);
    }
    mol.addBond(atoms[0], atoms[5], BondType::DoubleBond);
    return std::make_tuple(atoms[0], atoms[1], atoms[2], atoms[3], atoms[4], atoms[5]);
}

std::pair<atom_t, atom_t> JMol_p::makeAcyl(const ElementType &_acyl, const ElementType &_root) {
    auto acyl = mol.addAtom(_acyl);
    atom_t root;
    if (last_holder) {
        last_holder->setType(_root);
        last_holder->setCharge(0);
        root = last_holder;
        clearLastHolder();
    } else {
        root = mol.addAtom(_root);
    }
    mol.addBond(acyl, root, BondType::DoubleBond);
    return {root, root};
}

void xgd::initSuperAtomMap() {
    if (!SUPER_ATOM_MAP.empty()) { return; }
    // 元素语义
    for (auto&[str, data]:STR_ELEMENT_MAP) {
        SUPER_ATOM_MAP[str] = TokenType::Element;
    }
    // 数字语义
    for (int i = 0; i <= 9; i++) {
        SUPER_ATOM_MAP[std::to_string(i)] = TokenType::Number;
    }
    // 电荷语义
    SUPER_ATOM_MAP["+"] = TokenType::Pos;
    SUPER_ATOM_MAP["_"] = TokenType::Neg;
    // 化学键语义
    SUPER_ATOM_MAP["-"] = TokenType::Single;
    SUPER_ATOM_MAP["="] = TokenType::Double;
    SUPER_ATOM_MAP["#"] = TokenType::Triple;
    // 括号语义
    SUPER_ATOM_MAP["("] = TokenType::Left;
    SUPER_ATOM_MAP[")"] = TokenType::Right;
    // 缩写语义
    SUPER_ATOM_MAP["Me"] = TokenType::Me;
    SUPER_ATOM_MAP["C2H5"] = SUPER_ATOM_MAP["C2H4"] = SUPER_ATOM_MAP["Et"] = SUPER_ATOM_MAP["Ethyl"] = TokenType::Et;
    SUPER_ATOM_MAP["C3H6"] = SUPER_ATOM_MAP["C3H7"] = SUPER_ATOM_MAP["nPr"] = SUPER_ATOM_MAP["Pr"] = TokenType::nPr;
    SUPER_ATOM_MAP["iPr"] = TokenType::iPr;
    SUPER_ATOM_MAP["iBu"] = TokenType::iBu;
    SUPER_ATOM_MAP["tBu"] = TokenType::tBu;
    SUPER_ATOM_MAP["C4H8"] = SUPER_ATOM_MAP["C4H9"] = SUPER_ATOM_MAP["nBu"] = SUPER_ATOM_MAP["Bu"] = TokenType::nBu;
    SUPER_ATOM_MAP["Am"] = TokenType::Am;// 正戊基
    SUPER_ATOM_MAP["NO2"] = TokenType::NO2;
    SUPER_ATOM_MAP["NO"] = TokenType::NO;
    SUPER_ATOM_MAP["SO3"] = TokenType::SO3;
    SUPER_ATOM_MAP["SO2"] = SUPER_ATOM_MAP["SOO"] = TokenType::SO2;
    SUPER_ATOM_MAP["SO"] = TokenType::SO;
    SUPER_ATOM_MAP["CSO"] = TokenType::CSO;
    SUPER_ATOM_MAP["COS"] = TokenType::COS;
    SUPER_ATOM_MAP["CO2"] = SUPER_ATOM_MAP["COO"] = SUPER_ATOM_MAP["OOC"] = SUPER_ATOM_MAP["Ac"] = TokenType::CO2;
    SUPER_ATOM_MAP["CO"] = TokenType::CO;
    SUPER_ATOM_MAP["Ph"] = TokenType::Ph;
    SUPER_ATOM_MAP["Bz"] = TokenType::Bz;// 苯酰基
    SUPER_ATOM_MAP["Bn"] = TokenType::Bn;// 苯甲基
    SUPER_ATOM_MAP["Z"] = SUPER_ATOM_MAP["Cbz"] = TokenType::Cbz;// 苯甲氧酰基
    SUPER_ATOM_MAP["NC"] = SUPER_ATOM_MAP["CN"] = TokenType::CN;
    SUPER_ATOM_MAP["SCN"] = TokenType::SCN;
    // 只支持部分烷基、氨基缩写，TODO: 这个需要写正则
    SUPER_ATOM_MAP["N2H4"] = TokenType::N2H4;
    SUPER_ATOM_MAP["N2H5"] = TokenType::N2H5;
    // TODO:
    SUPER_ATOM_MAP["OCN"] = SUPER_ATOM_MAP["NCO"] = TokenType::OCN;
    SUPER_ATOM_MAP["NCS"] = TokenType::NCS;
    SUPER_ATOM_MAP["Ace"] = SUPER_ATOM_MAP["Acetyl"] = SUPER_ATOM_MAP["Acyl"] = TokenType::Ace;
    SUPER_ATOM_MAP["THPO"] = TokenType::THPO;
    SUPER_ATOM_MAP["NHZ"] = TokenType::NHZ;
    SUPER_ATOM_MAP["Ms"] = TokenType::Ms;

    // Map 里面最长字符串的长度
    for (const auto&[str, _]:SUPER_ATOM_MAP) {
        if (MAX_SUPER_ATOM_LENGTH < str.length()) { MAX_SUPER_ATOM_LENGTH = str.length(); }
    }
}
//#include <iostream>

std::optional<JMol_p::token_struct> JMol_p::interpret(const std::string &inputName) {
    std::vector<TokenType> tokenVec;
    std::unordered_map<size_t, ElementType> elementMap;
    std::unordered_map<size_t, int> numberMap;
    size_t it = 0;
    while (it < inputName.length()) {
        size_t offset = (std::min)(inputName.length() - it, MAX_SUPER_ATOM_LENGTH);
//        std::cout << "offset=" << offset << std::endl;
        std::string subStr = inputName.substr(it, offset);
        auto its = SUPER_ATOM_MAP.find(subStr);
        while (SUPER_ATOM_MAP.end() == its && offset >= 1) {
            offset -= 1;
            subStr = inputName.substr(it, offset);
            its = SUPER_ATOM_MAP.find(subStr);
        }
        if (SUPER_ATOM_MAP.end() == its) { return std::nullopt; }
        tokenVec.push_back(its->second);
        if (isNumberToken(its->second)) {
            size_t idx = it + offset;
            while (idx < inputName.length() && '0' <= inputName[idx] && inputName[idx] <= '9') {
                ++offset;
                ++idx;
            }
            subStr = inputName.substr(it, offset);
            numberMap[tokenVec.size() - 1] = std::stoi(subStr);
        } else if (isElementToken(its->second)) {
            elementMap[tokenVec.size() - 1] = STR_ELEMENT_MAP[subStr];
        }
        it += offset;
//        std::cout << subStr << std::endl;
    }
    // 这里最好 move 一下，防止编译器没实现这种优化
    return std::make_tuple(std::move(tokenVec), std::move(numberMap), std::move(elementMap));
}

std::pair<atom_t, atom_t> JMol_p::extractNoBracketTokens(
        token_struct &tokenStruct, size_t iBeg, size_t iEnd, int suffix, atom_t parent) {
//    qDebug() << __FUNCTION__ << "suffix=" << suffix;
    auto&[tokens, numbers, elements]=tokenStruct;
    atom_t a_beg0 = nullptr, a_end0 = nullptr;
    // 如果有要被挂载的原子，那么不可能原位修改接入原子
    if (parent) { clearLastHolder(); }
    while (suffix--) {
        // 解析 [iBeg, iEnd]，顺序生成 1 个子图，需要维护 a_beg, a_end
        // 迭代结束后维护 a_beg0, a_end0
        BondType lastBond = BondType::SingleBond;
        atom_t last_ele, a_beg, a_end;
        last_ele = a_beg = a_end = nullptr;
        for (size_t i = iBeg; i <= iEnd; i++) {
            atom_t a1, a2;
            int number = 1;
            bool eleBindHappen = false;
            auto &curToken = tokens[i];
            if (isElementToken(curToken)) {
                // 拼接元素
                if (i + 1 <= iEnd) {
                    auto &nextToken = tokens[i + 1];
                    if (isNumberToken(nextToken)) {
                        number = numbers[i + 1];
//                        qDebug() << "number=" << number;
                        std::tie(a1, a2) = makeElementType(
                                elements[i], last_ele, number);
                        i += 1;
                        eleBindHappen = true;
                    } else {
                        std::tie(a1, a2) = makeElementType(elements[i]);
                        if (a1 && a1->getCommonNebNum() > 1) {
                            // 如果遇到可续接的原子，更新主原子信息
//                            qDebug() << "bind" << a1->getQName();
                            last_ele = a1;
                        }
                    }
                } else {
                    std::tie(a1, a2) = makeElementType(elements[i]);
                    if (a1 && a1->getCommonNebNum() > 1) {
//                        qDebug() << "bind" << a1->getQName();
                        last_ele = a1;
                    }
                }
            } else if (isAbbToken(curToken)) {
                // 拼接缩略词
//                qDebug() << __FUNCTION__ << "isAbbToken:" << (int) curToken;
                std::tie(a1, a2) = makeAbbType(curToken);
            } else if (isChargeToken(curToken)) {
                // 向主原子划归电荷，FIXME: 这里是一个粗糙的实现
                if (last_ele) {
                    last_ele->setCharge(curToken == TokenType::Pos ? 1 : -1);
                }
            }
            if (isBondToken(curToken)) {
                lastBond = curToken == TokenType::Triple ? BondType::TripleBond :
                           curToken == TokenType::Double ? BondType::DoubleBond :
                           BondType::SingleBond;
            } else if (!eleBindHappen && a1 && a2) {
                // 没有需要挂载的原子
                if (!a_beg) {
                    a_beg = a1;
                    a_end = a2;
                } else {
                    if (a_end && a1) {
                        mol.addBond(a_end, a1, lastBond);
                        // 如果遇到了新原子，那么之前遇到的化学键一定使用过了，此处重置
                        lastBond = BondType::SingleBond;
                        a_end = a2;
                    }
                }
            }
        }
        // 如果前缀原子存在，那么把整个子图挂载到前缀原子
        if (parent && a_beg) {
            // 默认以单键接入上一个节点
            mol.addBond(parent, a_beg, BondType::SingleBond);
        } else if (!a_beg0) {// 第 1 个子图
            a_beg0 = a_beg;
            a_end0 = a_end;
        } else {// 后续子图
            if (a_end0 && a_beg) {
                // 默认以单键接入上一个节点
                mol.addBond(a_end0, a_beg, BondType::SingleBond);
            }
            a_end0 = a_end;
        }
    }
    if (parent) { return {parent, parent}; }
    else {
        return {a_beg0, a_end0};
    }
}

/**
 * 我在毕设里要达成的目标：基于字符串解析的残基拆解，顺序解析
 * 1、支持单层括号，支持下标数字，支持化学键符号，支持单电荷
 * 2、不支持谓词语义（包括左计数和异构符），不支持多电荷
 * TODO:
 * 1、电荷解析通过词法分析做
 * 2、没有分隔符，请使用最长单词试探法
 */
bool xgd::JMol_p::tryExpand(const id_type &_aid) {
    initSuperAtomMap();
    auto atom = mol.getAtom(_aid);
    if (!atom) { return false; }
    if (ElementType::SA != atom->getType()) { return false; }
    std::string inputName = atom->getName();
//    qDebug() << __FUNCTION__ << inputName.c_str();
    auto opt = interpret(inputName);
    if (!opt) { return false; }
    auto &tokenStruct = opt.value();
    qDebug() << atom->isLeftToRight();
    if (atom->isLeftToRight()) {
        // reverse tokenStruct here
        if (!reverseTokens(tokenStruct)) { return false; }
    }
    auto&[tokens, numbers, elements]=tokenStruct;
    bindLastHolder(atom);// 原位修改起始原子
    atom_t a_end = atom, a1, a2;
    int number;
    size_t beg = 0, end = 0, iBeg, iEnd;
    while (beg < tokens.size()) {
        auto &token = tokens[beg];
        // 遇到左括号
        if (isLeftToken(token)) {
            end = beg + 1;
            // 找到第一个右括号
            while (end < tokens.size() && !isLeftToken(tokens[end]) && !isRightToken(tokens[end])) { ++end; }
            if (end >= tokens.size()) { return false; }
            if (!isRightToken(tokens[end - 1])) { return false; }
            iBeg = beg + 1;
            iEnd = end - 2;
            // 如果右括号附着有数字，记下这个数字
            if (end + 1 < tokens.size() && isNumberToken(tokens[end + 1])) {
                end += 1;
                iEnd += 1;
                number = numbers[end];
            } else {
                number = 1;
            }
            bool isHangOn = false;
            // 是否附着在前一个原子上
            if (number <= 3) {
                isHangOn = true;
            }
            // 附着情况，把括号里面的子图重复k次挂在前一个原子上
            atom_t preAtom = isHangOn ? a_end : nullptr;
            std::tie(a1, a2) = extractNoBracketTokens(
                    tokenStruct, iBeg, iEnd, number, preAtom);
            if (!(a1 && a2)) { return false; }
            // 非附着情况，需要手动连接子图和前一个原子
            if (!isHangOn && a_end && a1) {
                mol.addBond(a_end, a1);
            }
            a_end = a2;
            beg = end;
        } else {
            end = beg + 1;
            // 寻找一个最长的连续的不包括括号的字符串
            while (end < tokens.size() && !isLeftToken(tokens[end]) && !isRightToken(tokens[end])) { ++end; }
            iBeg = beg;
            iEnd = end - 1;
            bool isFirst = last_holder != nullptr;
            std::tie(a1, a2) = extractNoBracketTokens(
                    tokenStruct, iBeg, iEnd, 1, nullptr);
            if (!(a1 && a2)) { return false; }
            // 如果不是接入点原子，那么需要把新建子图连接到原分子图
            if (!isFirst && a_end && a1) {
                mol.addBond(a_end, a1);
            }
            a_end = a2;
            beg = end;
        }
    }
    // TODO: 解决多点接入问题
//        if (atom && a_end) {
//            auto &bonds = atom->getSaBonds();
//            qDebug() << "bonds.size()=" << bonds.size();
//            if (bonds.size() > 1) {
//                std::sort(bonds.begin(), bonds.end(), [](
//                        const std::pair<float, std::shared_ptr<JBond>> &a,
//                        const std::pair<float, std::shared_ptr<JBond>> &b) {
//                    return a.first < b.first;
//                });
//                mol.exceedAllData();
//                if (bonds[1].second->getFrom()->getId() == atom->getId()) {
//                    bonds[1].second->setFrom(a_end);
//                } else {
//                    bonds[1].second->setTo(a_end);
//                }
//                mol.rebuildAllData();
//            }
//        }



//    qDebug() << __FUNCTION__ << "return true";
    return true;
}

void JMol_p::bindLastHolder(JMol_p::atom_t atom) {
    last_holder = std::move(atom);
}

void JMol_p::clearLastHolder() {
    last_holder = nullptr;
}

std::pair<atom_t, atom_t> JMol_p::makeElementType(
        const ElementType &_ele, atom_t parent, int num) {
    while (num--) {
        auto atom = mol.addAtom(_ele);
        if (parent && atom) {
            mol.addBond(parent, atom, BondType::SingleBond);
        }
    }
    return {parent, parent};
}

bool JMol_p::reverseTokens(JMol_p::token_struct &tokenStruct) {
    token_struct newStruct;
    auto&[tokens, numbers, elements]=tokenStruct;
    auto&[tokens2, numbers2, elements2]=newStruct;
    std::unordered_map<size_t, size_t> numberMap, elementMap;
    // 我们只支持单层括号
    // H3C-COO-(CH3COOEt)-CH2CH3-H2C
    int numCache = -1;
    int numBracketHolder = -1;
    for (int i = tokens.size() - 1; i >= 0; i--) {
        auto &curToken = tokens[i];
        if (i - 1 >= 0) {
            int j = i - 1;
            if (isNumberToken(tokens[j])) {
                numCache = numbers[j];
            }
        }
        if (isNumberToken(curToken)) {

        } else if (isRightToken(curToken)) {
            tokens2.push_back(TokenType::Left);
        } else if (isLeftToken(curToken)) {
            tokens2.push_back(TokenType::Right);
        } else if (!isNumberToken(curToken)){
            tokens2.push_back(curToken);
        }
    }
    // 恢复数字、元素记录表
    for (size_t i = 0; i < tokens2.size(); i++) {
        auto &curToken = tokens[i];
        if (isNumberToken(curToken)) {
            numbers2[i] = numbers[numberMap[i]];
        } else if (isElementToken(curToken)) {
            elements2[i] = elements[elementMap[i]];
        }
    }
    std::swap(newStruct, tokenStruct);
    return false;
}
