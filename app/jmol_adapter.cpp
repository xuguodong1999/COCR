#include "jmol_adapter.hpp"
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/mol.h>

xgd::JMolAdapter::JMolAdapter() {
    obMol = new OpenBabel::OBMol();
}

xgd::JMolAdapter::~JMolAdapter() {
    delete obMol;
    obMol = nullptr;
}

xgd::JMolAdapter::JMolAdapter(const xgd::JMolAdapter &_jMolAdapter) {
    obMol = new OpenBabel::OBMol(*(_jMolAdapter.obMol));
}

xgd::JMolAdapter::JMolAdapter(xgd::JMolAdapter &&_jMolAdapter) {
    obMol = _jMolAdapter.obMol;
}

std::shared_ptr<xgd::JAtom> xgd::JMolAdapter::removeAtom(const size_t &_aid, bool _check) {
    auto atom = JMol::removeAtom(_aid, _check);
    if (!atom)return atom;
    auto obAtom = atomIdMap.find(atom->getId());
    if (atomIdMap.end() != obAtom) {
        obMol->DeleteAtom(obAtom->second);
        atomIdMap.erase(atom->getId());
        atomIdMap2.erase(obAtom->second);
    }
    return atom;
}

std::shared_ptr<xgd::JBond> xgd::JMolAdapter::removeBond(const size_t &_bid, bool _check) {
    auto bond = JMol::removeBond(_bid, _check);
    if (!bond)return nullptr;
    auto obBond = bondIdMap.find(bond->getId());
    if (bondIdMap.end() != obBond) {
        obMol->DeleteBond(obBond->second);
        bondIdMap.erase(bond->getId());
        bondIdMap2.erase(obBond->second);
    }
    return bond;
}

std::shared_ptr<xgd::JResidue> xgd::JMolAdapter::removeResidue(const size_t &_rid, bool _check) {
    // TODO: handle residue in openbabel
    return JMol::removeResidue(_rid, _check);
}

std::shared_ptr<xgd::JResidue> xgd::JMolAdapter::addResidue(
        const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y) {
    // TODO: handle residue in openbabel
    return JMol::addResidue(_text, _isLeftToRight, _x, _y);
}

std::shared_ptr<xgd::JResidue> xgd::JMolAdapter::addResidue(
        const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y, const float &_z) {
    // TODO: handle residue in openbabel
    return JMol::addResidue(_text, _isLeftToRight, _x, _y, _z);
}

std::shared_ptr<xgd::JBond> xgd::JMolAdapter::addBond(
        std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2) {
    auto bond = JMol::addBond(_a1, _a2);
    if (!bond)return nullptr;
    auto obBond = obMol->NewBond();
    bondIdMap[bond->getId()] = obBond;
    bondIdMap2[obBond] = bond->getId();
    int obBondOrder;
    switch (bond->getType()) {
        case BondType::SingleBond:
            obBondOrder = 1;
            break;
        case BondType::DoubleBond:
            obBondOrder = 2;
            break;
        case BondType::TripleBond:
            obBondOrder = 3;
            break;
        case BondType::DelocalizedBond:
            obBond->SetAromatic(true);
            obBondOrder = 5;
            break;
        case BondType::UpBond:
            obBond->SetWedge(true);
            obBondOrder = 1;
            break;
        case BondType::DownBond:
            obBond->SetHash(true);
            obBondOrder = 1;
            break;
        case BondType::ImplicitBond:
            obBond->SetWedgeOrHash(true);
            obBondOrder = 1;
            break;
        default:
            throw std::runtime_error("BondType obTo OpenBabel::BondOrder not Implemented!");
    }
    obBond->SetBondOrder(obBondOrder);
    auto from = bond->getFrom();
    if (from) {
        auto obFrom = atomIdMap.find(from->getId());
        if (atomIdMap.end() != obFrom) {
            obBond->SetBegin(obFrom->second);
        }
    }
    auto to = bond->getTo();
    if (to) {
        auto obTo = atomIdMap.find(to->getId());
        if (atomIdMap.end() != obTo) {
            obBond->SetEnd(obTo->second);
        }
    }
    return bond;
}

std::shared_ptr<xgd::JAtom> xgd::JMolAdapter::addAtom(
        const xgd::ElementType &_element, const float &_x, const float &_y) {
    auto atom = JMol::addAtom(_element, _x, _y);
    if (!atom)return nullptr;
    auto obAtom = obMol->NewAtom();
    atomIdMap[atom->getId()] = obAtom;
    atomIdMap2[obAtom] = atom->getId();
    return atom;
}

std::shared_ptr<xgd::JAtom> xgd::JMolAdapter::addAtom(
        const xgd::ElementType &_element, const float &_x, const float &_y, const float &_z) {
    auto atom = addAtom(_element, _x, _y);
    if (!atom)return nullptr;
    atom->set3D(_x, _y, _z);
    return atom;
}

std::string xgd::JMolAdapter::writeAsPDB() const {
    return std::string();
}

std::string xgd::JMolAdapter::writeAs(const std::string &_formatSuffix) const {
    return std::string();
}

