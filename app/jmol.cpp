#include "jmol.hpp"


using namespace xgd;

std::shared_ptr<JAtom> xgd::JMol::getAtom(const size_t &_aid) {
    if (_aid >= atomVec.size())return nullptr;
    return atomVec[_aid];
}

std::shared_ptr<JBond> xgd::JMol::getBond(const size_t &_bid) {
    if (_bid >= bondVec.size())return nullptr;
    return bondVec[_bid];
}

std::shared_ptr<JResidue> xgd::JMol::getResidue(const size_t &_rid) {
    if (_rid >= residueVec.size())return nullptr;
    return residueVec[_rid];
}

std::shared_ptr<JAtom> JMol::addAtom(const ElementType &_element, const float &_x, const float &_y) {
    auto atom = std::make_shared<JAtom>(atomVec.size(), _element, _x, _y);
    atomVec.push_back(atom);
    return atom;
}

std::shared_ptr<JAtom> JMol::addAtom(
        const ElementType &_element, const float &_x, const float &_y, const float &_z) {
    auto atom = addAtom(_element, _x, _y);
    if (!atom)return nullptr;
    atom->set3D(_x, _y, _z);
    return atom;
}

std::shared_ptr<JBond> JMol::addBond(std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2) {
    return std::shared_ptr<JBond>();
}

std::shared_ptr<JBond> JMol::addBond(const size_t &_aid1, const size_t &_aid2) {
    return std::shared_ptr<JBond>();
}

std::shared_ptr<JResidue>
JMol::addResidue(const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y) {
    auto residue = std::make_shared<JResidue>(residueVec.size(), _text, _isLeftToRight, _x, _y);
    residueVec.push_back(residue);
    return residue;
}

std::shared_ptr<JResidue> JMol::addResidue(
        const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y, const float &_z) {
    auto residue = addResidue(_text, _isLeftToRight, _x, _y);
    if (!residue)return nullptr;
    residue->set3D(_x, _y, _z);
    return residue;
}

std::shared_ptr<JAtom> JMol::removeAtom(const size_t &_aid) {
    if (_aid >= atomVec.size())return nullptr;
    auto atom = atomVec[_aid];
    atomVec[_aid] = nullptr;
    return atom;
}

std::shared_ptr<JBond> JMol::removeBond(const size_t &_bid) {
    if (_bid >= bondVec.size())return nullptr;
    auto bond = bondVec[_bid];
    bondVec[_bid] = nullptr;
    return bond;
}

std::shared_ptr<JResidue> JMol::removeResidue(const size_t &_rid) {
    if (_rid >= residueVec.size())return nullptr;
    auto residue = residueVec[_rid];
    residueVec[_rid] = nullptr;
    return residue;
}

void JMol::setId(const size_t &_id) {
    id = _id;
}

size_t JMol::getId() {
    return id;
}

JMol::JMol() : id(0), is3DInfoLatest(false), is2DInfoLatest(false) {

}

void JMol::loopAtomVec(std::function<void(JAtom &)> _func) {
    for (auto &atom:atomVec) {
        if (atom)_func(*atom);
    }
}

void JMol::loopBondVec(std::function<void(JBond &)> _func) {
    for (auto &bond:bondVec) {
        if (bond)_func(*bond);
    }
}

