#include "jmol_p.hpp"

using namespace xgd;

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
        // TODO
        TokenType::Bn, TokenType::Cbz, TokenType::OCN, TokenType::NCS,
        TokenType::Ace, TokenType::THPO, TokenType::NHZ, TokenType::Ms
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

void JMol_p::add_bond_order_for_atom(const id_type &_aid, const int &_order) {
    auto it = atomTotalBondOrderMap.find(_aid);
    if (atomTotalBondOrderMap.end() == it) {
        atomTotalBondOrderMap[_aid] = _order;
    } else {
        it->second += _order;
    }
}

void JMol_p::add_db_num_for_atom(const id_type &_aid, const int &_num) {
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

int JMol_p::get_atom_order(const id_type &_aid) {
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
            add_bond_order_for_atom(from->getId(), order);
            add_bond_order_for_atom(to->getId(), order);
        }
        if (order == 2) {
            add_db_num_for_atom(from->getId(), 1);
            add_db_num_for_atom(to->getId(), 1);
        }
    });
    isValenceDataLatest = true;
}

bool JMol_p::IsValenceDataLatest() const {
    return isValenceDataLatest;
}

JMol_p::JMol_p(JMol &_mol) : mol(_mol), isValenceDataLatest(false) {

}

std::pair<atom_t, atom_t> JMol_p::makeAbbType(const TokenType &_abb) {
    switch (_abb) {
        case TokenType::Me: {
            return makeAlkane(1);
        }
        case TokenType::Et: {
            auto[first, last]= makeAlkane(2);
            return {first, first};
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
            break;
        }
        case TokenType::NO: {
            break;
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
            auto c6 = makeBenzene();
            auto c = std::get<0>(c6);
            auto C = mol.addAtom(ElementType::C);
            mol.addBond(c, C);
            return {C, C};
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
        default: {
            break;
        }
    }
    return {nullptr, nullptr};
}

std::pair<atom_t, atom_t> JMol_p::makeElementType(const ElementType &_ele) {
    auto atom = mol.addAtom(_ele);
    return {atom, atom};
}

std::pair<atom_t, atom_t> JMol_p::makeAlkane(const int &_num) {
    if (_num <= 0) { return {nullptr, nullptr}; }
    auto first = mol.addAtom(ElementType::C);
    auto last = first;
    for (int i = 1; i < _num; i++) {
        auto next = mol.addAtom(ElementType::C);
        mol.addBond(last, next);
        last = next;
    }
    return {first, last};
}

std::tuple<atom_t, atom_t, atom_t, atom_t, atom_t, atom_t> JMol_p::makeBenzene() {
    atom_t atoms[6];
    atoms[0] = mol.addAtom(ElementType::C);
    for (int i = 1; i < 6; i++) {
        atoms[i] = mol.addAtom(ElementType::C);
        mol.addBond(atoms[i], atoms[i - 1], i % 2 ? BondType::SingleBond : BondType::DoubleBond);
    }
    mol.addBond(atoms[0], atoms[5], BondType::DoubleBond);
    return std::make_tuple(atoms[0], atoms[1], atoms[2], atoms[3], atoms[4], atoms[5]);
}

std::pair<atom_t, atom_t> JMol_p::makeAcyl(const ElementType &_acyl, const ElementType &_root) {
    auto a = mol.addAtom(_acyl);
    auto c = mol.addAtom(_root);
    mol.addBond(a, c, BondType::DoubleBond);
    return {c, c};
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
    SUPER_ATOM_MAP["Et"] = SUPER_ATOM_MAP["Ethyl"] = TokenType::Et;
    SUPER_ATOM_MAP["nPr"] = SUPER_ATOM_MAP["Pr"] = TokenType::nPr;
    SUPER_ATOM_MAP["iPr"] = TokenType::iPr;
    SUPER_ATOM_MAP["iBu"] = TokenType::iBu;
    SUPER_ATOM_MAP["tBu"] = TokenType::tBu;
    SUPER_ATOM_MAP["nBu"] = SUPER_ATOM_MAP["Bu"] = TokenType::nBu;
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
    SUPER_ATOM_MAP["Cbz"] = TokenType::Cbz;// 苯甲氧酰基
    // TODO:
    SUPER_ATOM_MAP["OCN"] = SUPER_ATOM_MAP["NCO"] = TokenType::OCN;
    SUPER_ATOM_MAP["NCS"] = SUPER_ATOM_MAP["SCN"] = TokenType::NCS;
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
    std::queue<ElementType> eleQueue;
    std::queue<int> numQue;
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
            numQue.push(std::stoi(subStr));
        } else if (isElementToken(its->second)) {
            eleQueue.push(STR_ELEMENT_MAP[subStr]);
        }
        it += offset;
//        std::cout << subStr << std::endl;
    }
    // 这里最好 move 一下，防止编译器没实现这种优化
    return std::make_tuple(std::move(tokenVec), std::move(numQue), std::move(eleQueue));
}

void JMol_p::extractNoBracketTokens(token_struct &tokenStruct, size_t iBeg, size_t iEnd,const int&suffix) {
    auto&[tokens, numbers, elements]=tokenStruct;
    // 解析 [iBeg, iEnd]
    atom_t a_beg = nullptr, a_end = nullptr;
    BondType lastBond = BondType::SingleBond;
    for (size_t i = iBeg; i <= iEnd; i++) {
        atom_t a1, a2;
        auto &curToken = tokens[i];
        if (isElementToken(curToken)) {
            // 拼接元素
            auto ele = elements.front();
            elements.pop();
            std::tie(a1, a2) = makeElementType(ele);
        } else if (isAbbToken(curToken)) {
            // 拼接缩略词
            std::tie(a1, a2) = makeAbbType(curToken);
        } else if (isChargeToken(curToken)) {
            // 划归电荷，FIXME: 下面是一个粗糙的实现
            if (a2) {
                a2->setCharge(curToken == TokenType::Pos ? 1 : -1);
            } else if (a1) {
                a1->setCharge(curToken == TokenType::Pos ? 1 : -1);
            }
        }
        if (isBondToken(curToken)) {
            lastBond = curToken == TokenType::Triple ? BondType::TripleBond :
                       curToken == TokenType::Double ? BondType::DoubleBond :
                       BondType::SingleBond;
        } else if (a1 && a2) {
            if (!a_beg) { a_beg = a1; }
            if (a_end) {
                mol.addBond(a_end, a1, lastBond);
                lastBond = BondType::SingleBond;// 如果遇到了新原子，那么之前遇到的化学键一定使用过了，此处重置
            }
            a_end = a2;
        }
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
    auto opt = interpret(inputName);
    if (!opt) { return false; }
    auto &tokenStruct = opt.value();
    auto&[tokens, numbers, elements]=tokenStruct;
    TokenType lastAbb = TokenType::None, curAbb;
    int number;
    int lbCount = 0;
    size_t beg = 0, end = 0, iBeg, iEnd;
    while (beg < tokens.size()) {
        auto &token = tokens[beg];
        if (isLeftToken(token)) {
            end = beg + 1;
            // find and skip right bracket here
            while (end < tokens.size() && !isRightToken(tokens[end])) { ++end; }
            iBeg = beg + 1;
            iEnd = (end == tokens.size()) ? end - 1 : end - 2;
            // 如果有数字，记下这个数字

            extractNoBracketTokens(tokenStruct, iBeg, iEnd);
            beg = end;
        } else {
            extractNoBracketTokens(tokenStruct, beg, beg);
            beg += 1;
        }

    }
    return true;
}
