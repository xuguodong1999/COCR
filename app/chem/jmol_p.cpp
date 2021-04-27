#include "jmol_p.hpp"

using namespace xgd;

std::unordered_map<std::string, AbbType> xgd::SUPER_ATOM_MAP;
size_t xgd::MAX_SUPER_ATOM_LENGTH = 0;

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

std::pair<atom_t, atom_t> JMol_p::makeAbbType(const AbbType &_abb) {
    switch (_abb) {
        case AbbType::Me: {
            return makeAlkane(1);
        }
        case AbbType::Et: {
            auto[first, last]= makeAlkane(2);
            return {first, first};
        }
        case AbbType::iPr: {
            auto[first, last]=makeAlkane(2);
            mol.addBond(first, mol.addAtom(ElementType::C));
            return {first, first};
        }
        case AbbType::nPr: {
            auto[first, last]= makeAlkane(3);
            return {first, first};
        }
        case AbbType::iBu: {
            auto[first, last]=makeAlkane(3);
            mol.addBond(first, mol.addAtom(ElementType::C));
            return {first, first};
        }
        case AbbType::tBu: {
            auto[first, last]=makeAlkane(2);
            mol.addBond(first, mol.addAtom(ElementType::C));
            mol.addBond(first, mol.addAtom(ElementType::C));
            return {first, first};
        }
        case AbbType::nBu: {
            auto[first, last]= makeAlkane(4);
            return {first, first};
        }
        case AbbType::Am: {
            auto[first, last]= makeAlkane(5);
            return {first, first};
        }
        case AbbType::NO2: {
            break;
        }
        case AbbType::NO: {
            break;
        }
        case AbbType::SO3: {
            auto[s, _]= makeAcyl(ElementType::O, ElementType::S);
            auto o1 = mol.addAtom(ElementType::O);
            mol.addBond(s, o1, BondType::DoubleBond);
            auto o2 = mol.addAtom(ElementType::O);
            mol.addBond(s, o2);
            return {s, o2};
        }
        case AbbType::SO2: {
            auto[s, _]= makeAcyl(ElementType::O, ElementType::S);
            auto o = mol.addAtom(ElementType::O);
            mol.addBond(s, o, BondType::DoubleBond);
            return {s, s};
        }
        case AbbType::SO: {
            return makeAcyl(ElementType::O, ElementType::S);
        }
        case AbbType::CO2: {
            auto[c, _]= makeAcyl(ElementType::O, ElementType::C);
            auto o = mol.addAtom(ElementType::O);
            mol.addBond(c, o);
            return {c, o};
        }
        case AbbType::CO: {
            return makeAcyl(ElementType::O, ElementType::C);
        }
        case AbbType::Ph: {
            auto c6 = makeBenzene();
            auto c = std::get<0>(c6);
            return {c, c};
        }
        case AbbType::Bn: {
            auto c6 = makeBenzene();
            auto c = std::get<0>(c6);
            auto C = mol.addAtom(ElementType::C);
            mol.addBond(c, C);
            return {C, C};
        }
        case AbbType::CSO: {
            auto[c, _]= makeAcyl(ElementType::S, ElementType::C);
            auto o = mol.addAtom(ElementType::O);
            mol.addBond(c, o);
            return {c, o};
        }
        case AbbType::COS: {
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
        SUPER_ATOM_MAP[str] = AbbType::Element;
    }
    // 数字语义
    for (int i = 0; i <= 9; i++) {
        SUPER_ATOM_MAP[std::to_string(i)] = AbbType::Number;
    }
    // 电荷语义
    SUPER_ATOM_MAP["+"] = AbbType::Pos;
    SUPER_ATOM_MAP["_"] = AbbType::Neg;
    // 化学键语义
    SUPER_ATOM_MAP["-"] = AbbType::Single;
    SUPER_ATOM_MAP["="] = AbbType::Double;
    SUPER_ATOM_MAP["#"] = AbbType::Triple;
    // 括号语义
    SUPER_ATOM_MAP["("] = AbbType::Left;
    SUPER_ATOM_MAP[")"] = AbbType::Right;
    // 缩写语义
    SUPER_ATOM_MAP["Me"] = AbbType::Me;
    SUPER_ATOM_MAP["Et"] = SUPER_ATOM_MAP["Ethyl"] = AbbType::Et;
    SUPER_ATOM_MAP["nPr"] = SUPER_ATOM_MAP["Pr"] = AbbType::nPr;
    SUPER_ATOM_MAP["iPr"] = AbbType::iPr;
    SUPER_ATOM_MAP["iBu"] = AbbType::iBu;
    SUPER_ATOM_MAP["tBu"] = AbbType::tBu;
    SUPER_ATOM_MAP["nBu"] = SUPER_ATOM_MAP["Bu"] = AbbType::nBu;
    SUPER_ATOM_MAP["Am"] = AbbType::Am;// 正戊基
    SUPER_ATOM_MAP["NO2"] = AbbType::NO2;
    SUPER_ATOM_MAP["NO"] = AbbType::NO;
    SUPER_ATOM_MAP["SO3"] = AbbType::SO3;
    SUPER_ATOM_MAP["SO2"] = SUPER_ATOM_MAP["SOO"] = AbbType::SO2;
    SUPER_ATOM_MAP["SO"] = AbbType::SO;
    SUPER_ATOM_MAP["CSO"] = AbbType::CSO;
    SUPER_ATOM_MAP["COS"] = AbbType::COS;
    SUPER_ATOM_MAP["CO2"] = SUPER_ATOM_MAP["COO"] = SUPER_ATOM_MAP["Ac"] = AbbType::CO2;
    SUPER_ATOM_MAP["CO"] = AbbType::CO;
    SUPER_ATOM_MAP["Ph"] = AbbType::Ph;
    SUPER_ATOM_MAP["Bz"] = AbbType::Bz;// 苯酰基
    SUPER_ATOM_MAP["Bn"] = AbbType::Bn;// 苯甲基
    SUPER_ATOM_MAP["Cbz"] = AbbType::Cbz;// 苯甲氧酰基
    // TODO:
    SUPER_ATOM_MAP["OCN"] = SUPER_ATOM_MAP["NCO"] = AbbType::OCN;
    SUPER_ATOM_MAP["NCS"] = SUPER_ATOM_MAP["SCN"] = AbbType::NCS;
    SUPER_ATOM_MAP["Ace"] = SUPER_ATOM_MAP["Acetyl"] = SUPER_ATOM_MAP["Acyl"] = AbbType::Ace;
    SUPER_ATOM_MAP["THPO"] = AbbType::THPO;
    SUPER_ATOM_MAP["NHZ"] = AbbType::NHZ;
    SUPER_ATOM_MAP["Ms"] = AbbType::Ms;

    for (const auto&[str, _]:SUPER_ATOM_MAP) {
        if (MAX_SUPER_ATOM_LENGTH < str.length()) { MAX_SUPER_ATOM_LENGTH = str.length(); }
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
    auto&[abbQue, numQue]=opt.value();
    AbbType lastAbb = AbbType::None, curAbb;
    while (!abbQue.empty()) {
        curAbb = abbQue.front();
        abbQue.pop();
        switch (curAbb) {
            case AbbType::Number: {
                int number = numQue.front();
                numQue.pop();
                break;
            }
        }
        lastAbb = curAbb;
    }
    return true;
}

#include <iostream>

using namespace std;

std::optional<std::pair<std::queue<AbbType>, std::queue<int>>>
JMol_p::interpret(const std::string &inputName) {
    std::queue<AbbType> abbQue;
    std::queue<int> numQue;
    size_t it = 0;
    while (it < inputName.length()) {
        size_t offset = (std::min)(inputName.length() - it, MAX_SUPER_ATOM_LENGTH);
        std::cout << "offset=" << offset << std::endl;
        std::string subStr = inputName.substr(it, offset);
        auto its = SUPER_ATOM_MAP.find(subStr);
        while (SUPER_ATOM_MAP.end() == its && offset >= 1) {
            offset -= 1;
            subStr = inputName.substr(it, offset);
            its = SUPER_ATOM_MAP.find(subStr);
        }
        if (SUPER_ATOM_MAP.end() != its) {
            abbQue.push(its->second);
            if (its->second == AbbType::Number) {
                size_t idx = it + offset;
                while (idx < inputName.length() && '0' <= inputName[idx] && inputName[idx] <= '9') {
                    ++offset;
                    ++idx;
                }
                subStr = inputName.substr(it, offset);
                numQue.push(std::stoi(subStr));
            }
            it += offset;
            std::cout << subStr << std::endl;
        } else {
            return std::nullopt;
        }
    }
    // 这里最好 move 一下，防止编译器没实现这种情况
    return std::make_pair(std::move(abbQue), std::move(numQue));
}
