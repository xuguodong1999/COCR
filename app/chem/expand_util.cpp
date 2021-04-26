#include "expand_util.hpp"

using namespace xgd;

std::unordered_map<std::string, int> xgd::SUPER_ATOM_MAP = {
        {"COOH",1},
        {"HOOC",1},

        {"COO_",2},
        {"_OOC",2},

        {"COOEt",3},
        {"EtOOC",3},

        {"COOMe",3},
        {"MeOOC",3},

        {"Et",4},
        {"Me",5},
};
/**
 * 处理分类：
 * 1、元素+卤素+数字+电荷类型，采用预定义所有可能的算法
 * 2、烷烃类型，同上
 * 3、硬编码类型
 */
bool xgd::tryExpand(JMol &_mol, std::shared_ptr<JAtom> _atom) {
    auto it=SUPER_ATOM_MAP.find(_atom->getName());
    if(SUPER_ATOM_MAP.end()==it){return false;}
    switch (it->second) {
        case 3:{
            break;
        }
        case 2:{
            break;
        }
        case 1:{
            break;
        }
        default:
            return false;
    }
    return true;
//    QString name = atom->getQName();
//    if (name == "COOH" || name == "HOOC") {
//        atom->setType(ElementType::C);
//        atom->setCharge(0);
//        auto o1 = addAtom(ElementType::O);
//        auto o2 = addAtom(ElementType::O);
//        addBond(atom, o1, BondType::SingleBond);
//        addBond(atom, o2, BondType::DoubleBond);
//        return;
//    }
//    if (name == "COOEt" || name == "EtOOC") {
//        atom->setType(ElementType::C);
//        atom->setCharge(0);
//        auto o1 = addAtom(ElementType::O);
//        auto o2 = addAtom(ElementType::O);
//        addBond(atom, o1, BondType::SingleBond);
//        addBond(atom, o2, BondType::DoubleBond);
//
//        auto c1 = addAtom(ElementType::C);
//        auto c2 = addAtom(ElementType::C);
//        addBond(c1, o1, BondType::SingleBond);
//        addBond(c1, c2, BondType::SingleBond);
//        return;
//    }
//    if (name == "COOMe" || name == "MeOOC") {
//        atom->setType(ElementType::C);
//        atom->setCharge(0);
//        auto o1 = addAtom(ElementType::O);
//        auto o2 = addAtom(ElementType::O);
//        addBond(atom, o1, BondType::SingleBond);
//        addBond(atom, o2, BondType::DoubleBond);
//
//        auto c1 = addAtom(ElementType::C);
//        addBond(c1, o1, BondType::SingleBond);
//        return;
//    }
//    if (name == "CHO" || name == "OHC") {
//        atom->setType(ElementType::C);
//        atom->setCharge(0);
//        auto o1 = addAtom(ElementType::O);
//        addBond(atom, o1, BondType::DoubleBond);
//        return;
//    }
//    if (name == "Me") {
//        atom->setType(ElementType::C);
//        atom->setCharge(0);
//        return;
//    }
//    if (name == "Et") {
//        atom->setType(ElementType::C);
//        atom->setCharge(0);
//        auto c1 = addAtom(ElementType::C);
//        addBond(atom, c1, BondType::SingleBond);
//        return;
//    }
//
//    decltype(STR_ELEMENT_MAP.end()) it;
//    for (int i = 0; i <= 4; i++) {
//        QString reg = "H";
//        if (i) { reg += QString::number(i); }
//        name = atom->getQName().replace(reg, "");
//        it = STR_ELEMENT_MAP.find(name.toStdString());
//        if (STR_ELEMENT_MAP.end() != it) {
//            atom->setType(it->second);
//            atom->setCharge(0);
//            return;
//        }
//    }
//
//    name = atom->getQName().replace("+", "");
//    it = STR_ELEMENT_MAP.find(name.toStdString());
//    if (STR_ELEMENT_MAP.end() != it) {
//        atom->setType(it->second);
//        atom->setCharge(1);
//        return;
//    }
//
//    name = atom->getQName().replace("_", "");
//    it = STR_ELEMENT_MAP.find(name.toStdString());
//    if (STR_ELEMENT_MAP.end() != it) {
//        atom->setType(it->second);
//        atom->setCharge(-1);
//        return;
//    }
}