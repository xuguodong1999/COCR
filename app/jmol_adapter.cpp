#include "jmol_adapter.hpp"
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/residue.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/obiter.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>

#include <coordgen/sketcherMinimizer.h>

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

std::shared_ptr<xgd::JAtom> xgd::JMolAdapter::removeAtom(const size_t &_aid) {
    onMolUpdated();
    auto atom = JMol::removeAtom(_aid);
    if (!atom)return atom;
    auto obAtom = atomIdMap.find(atom->getId());
    if (atomIdMap.end() != obAtom) {
        obMol->DeleteAtom(obAtom->second);
        atomIdMap.erase(atom->getId());
        atomIdMap2.erase(obAtom->second);
    }
    return atom;
}

std::shared_ptr<xgd::JBond> xgd::JMolAdapter::removeBond(const size_t &_bid) {
    onMolUpdated();
    auto bond = JMol::removeBond(_bid);
    if (!bond)return nullptr;
    auto obBond = bondIdMap.find(bond->getId());
    if (bondIdMap.end() != obBond) {
        obMol->DeleteBond(obBond->second);
        bondIdMap.erase(bond->getId());
        bondIdMap2.erase(obBond->second);
    }
    return bond;
}

std::shared_ptr<xgd::JResidue> xgd::JMolAdapter::removeResidue(const size_t &_rid) {
    onMolUpdated();
    // TODO: handle residue in openbabel
    return JMol::removeResidue(_rid);
}

std::shared_ptr<xgd::JResidue> xgd::JMolAdapter::addResidue(
        const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y) {
    onMolUpdated();
    // TODO: handle residue in openbabel
    return JMol::addResidue(_text, _isLeftToRight, _x, _y);
}

std::shared_ptr<xgd::JResidue> xgd::JMolAdapter::addResidue(
        const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y, const float &_z) {
    onMolUpdated();
    // TODO: handle residue in openbabel
    return JMol::addResidue(_text, _isLeftToRight, _x, _y, _z);
}

std::shared_ptr<xgd::JBond> xgd::JMolAdapter::addBond(
        std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2, const BondType &_type) {
    onMolUpdated();
    auto bond = JMol::addBond(_a1, _a2, _type);
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
    onMolUpdated();
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

std::string xgd::JMolAdapter::writeAsPDB() {
    return writeAs("pdb");
}

std::string xgd::JMolAdapter::writeAs(const std::string &_formatSuffix) {
    OpenBabel::OBConversion conv;
    auto formatOut = conv.FindFormat(_formatSuffix);
    if (!formatOut || !conv.SetOutFormat(formatOut)) {
        throw std::runtime_error("unknown format suffix: " + _formatSuffix);
    }
    if (!is3DInfoLatest) {
        if (!generate3D())
            throw std::runtime_error("fail to generate 3d");
    }
    return conv.WriteString(obMol);
}

std::string xgd::JMolAdapter::writeAsSMI() {
    return writeAs("can");
}

void xgd::JMolAdapter::readAsPDB(const std::string &_pdbBuffer) {
    readAs(_pdbBuffer, "pdb");
}

void xgd::JMolAdapter::readAsSMI(const std::string &_pdbBuffer) {
    readAs(_pdbBuffer, "smi");
}

void xgd::JMolAdapter::readAs(const std::string &_dataBuffer, const std::string &_formatSuffix) {
    onMolUpdated();
    OpenBabel::OBConversion conv;
    auto formatIn = conv.FindFormat(_formatSuffix);
    if (!formatIn || !conv.SetInFormat(formatIn)) {
        throw std::runtime_error("unknown readable format suffix: " + _formatSuffix);
    }
    std::stringstream ssm(_dataBuffer);
    if (!conv.Read(obMol, &ssm)) {
        throw std::runtime_error("fail to read buffer as format suffix: " + _formatSuffix);
    }
    syncNewEntityFromOBMol();
    if (obMol->Has3D()) {
        sync3D();
    }
}

void xgd::JMolAdapter::onMolUpdated() {
    is3DInfoLatest = is2DInfoLatest = false;
}

bool xgd::JMolAdapter::runForcefield() {
    if (obMol->Empty())return true;
    try {
        OpenBabel::OBBuilder builder;
        builder.Build(*obMol);
    } catch (...) {
        return false;
    }
    auto pFF = static_cast<OpenBabel::OBForceField *>(OpenBabel::OBPlugin::GetPlugin("forcefields", "uff"));
    if (!pFF) {
        return false;
    }
    pFF->SetLogLevel(OBFF_LOGLVL_NONE);
    if (!pFF->Setup(*obMol)) {
        std::cerr << "runOBForceField: setup force field ret false" << std::endl;
    }
    try {
        pFF->SteepestDescent(100, 1.0e-4);
        pFF->WeightedRotorSearch(10, 200);
        pFF->SteepestDescent(100, 1.0e-6);
        pFF->UpdateCoordinates(*obMol);
    } catch (...) {
        return false;
    }
    sync3D();
    return true;
}

void xgd::JMolAdapter::syncAtoms(std::function<void(xgd::JAtom &, OpenBabel::OBAtom *)> _func) {
    for (auto&[aid, obAtom]:atomIdMap) {
        auto atom = getAtom(aid);
        if (atom)
            _func(*atom, obAtom);
    }
}

void xgd::JMolAdapter::sync3D() {
    auto sync_3d = [&](xgd::JAtom &atom, OpenBabel::OBAtom *obAtom) -> void {
        atom.set3D(obAtom->x(), obAtom->y(), obAtom->z());
    };
    syncAtoms(sync_3d);
    is3DInfoLatest = true;
}

bool xgd::JMolAdapter::generate2D() {
    try {
        sketcherMinimizer minimizer;
        auto cMol = new sketcherMinimizerMolecule();
        std::unordered_map<JAtom *, sketcherMinimizerAtom *> tmpAtomMap;
        loopAtomVec([&](JAtom &atom) {
            auto cAtom = cMol->addNewAtom();
            cAtom->setAtomicNumber(atom.getAtomicNumber());
            tmpAtomMap[&atom] = cAtom;
        });
        std::vector<sketcherMinimizerBond *> cBondVec;
        loopBondVec([&](JBond &bond) {
            auto cBond = cMol->addNewBond(tmpAtomMap[&*bond.getFrom()], tmpAtomMap[&*bond.getTo()]);
            cBond->setBondOrder(bond.getBondOrder());
            cBondVec.push_back(cBond);
        });
        minimizer.initialize(cMol);
        minimizer.runGenerateCoordinates();
        for (auto&[atom, cAtom]:tmpAtomMap) {
            auto pos = cAtom->getCoordinates();
            atom->set2D(pos.x(), pos.y());
        }
    } catch (...) {
        return false;
    }
    is2DInfoLatest = true;
    return true;
}

bool xgd::JMolAdapter::generate3D() {
    return runForcefield();
}

void xgd::JMolAdapter::syncNewEntityFromOBMol() {
    FOR_ATOMS_OF_MOL(obAtomIter, *obMol) {
        auto obAtom = obAtomIter.operator->();
        auto it = atomIdMap2.find(obAtom);
        if (atomIdMap2.end() == it) {
            onMolUpdated();
            auto atom = JMol::addAtom(obAtom->GetAtomicNum());
            if (atom) {
                atomIdMap[atom->getId()] = obAtom;
                atomIdMap2[obAtom] = atom->getId();
            }
        }
    }
    FOR_BONDS_OF_MOL(obBondIter, *obMol) {
        auto obBond = obBondIter.operator->();
        auto it = bondIdMap2.find(obBond);
        if (bondIdMap2.end() == it) {
            onMolUpdated();
            auto bond = JMol::addBond(atomIdMap2[obBond->GetBeginAtom()], atomIdMap2[obBond->GetEndAtom()]);
            if (bond) {
                if (5 == obBond->GetBondOrder()) {
                    bond->setType(BondType::DelocalizedBond);
                } else if (obBond->IsHash()) {
                    bond->setType(BondType::DownBond);
                } else if (obBond->IsWedge()) {
                    bond->setType(BondType::UpBond);
                } else if (obBond->IsWedgeOrHash()) {
                    bond->setType(BondType::ImplicitBond);
                } else {
                    bond->setOrder(obBond->GetBondOrder());
                }
                bondIdMap[bond->getId()] = obBond;
                bondIdMap2[obBond] = bond->getId();
            }
        }
    }
}


