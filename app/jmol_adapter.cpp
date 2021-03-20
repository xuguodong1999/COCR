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

using namespace xgd;

JMolAdapter::JMolAdapter() : isOBMolLatest(true) {
    obMol = new OpenBabel::OBMol();
}

JMolAdapter::~JMolAdapter() {
    delete obMol;
    obMol = nullptr;
}

JMolAdapter::JMolAdapter(const JMolAdapter &_jMolAdapter) {
    obMol = new OpenBabel::OBMol(*(_jMolAdapter.obMol));
}

JMolAdapter::JMolAdapter(JMolAdapter &&_jMolAdapter) {
    obMol = _jMolAdapter.obMol;
}

std::shared_ptr<JAtom> JMolAdapter::removeAtom(const size_t &_aid) {
    onMolUpdated();
    isOBMolLatest = false;
    return JMol::removeAtom(_aid);
}

std::shared_ptr<JBond> JMolAdapter::removeBond(const size_t &_bid) {
    onMolUpdated();
    isOBMolLatest = false;
    return JMol::removeBond(_bid);
}

std::shared_ptr<JResidue> JMolAdapter::removeResidue(const size_t &_rid) {
    onMolUpdated();
    isOBMolLatest = false;
    // TODO: handle residue in openbabel
    return JMol::removeResidue(_rid);
}

std::shared_ptr<JResidue> JMolAdapter::addResidue(
        const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y) {
    onMolUpdated();
    checkOBMol();
    // TODO: handle residue in openbabel
    return JMol::addResidue(_text, _isLeftToRight, _x, _y);
}

std::shared_ptr<JResidue> JMolAdapter::addResidue(
        const std::string &_text, bool _isLeftToRight, const float &_x, const float &_y, const float &_z) {
    onMolUpdated();
    checkOBMol();
    // TODO: handle residue in openbabel
    return JMol::addResidue(_text, _isLeftToRight, _x, _y, _z);
}

std::shared_ptr<JBond> JMolAdapter::addBond(
        std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2, const BondType &_type) {
    onMolUpdated();
    checkOBMol();
    auto bond = JMol::addBond(_a1, _a2, _type);
    if (!bond)return nullptr;
    addOBBond(*bond);
    return bond;
}

std::shared_ptr<JAtom> JMolAdapter::addAtom(
        const ElementType &_element, const float &_x, const float &_y) {
    onMolUpdated();
    checkOBMol();
    auto atom = JMol::addAtom(_element, _x, _y);
    if (!atom)return nullptr;
    addOBAtom(*atom);
    return atom;
}

std::shared_ptr<JAtom> JMolAdapter::addAtom(
        const ElementType &_element, const float &_x, const float &_y, const float &_z) {
    auto atom = addAtom(_element, _x, _y);
    if (!atom)return nullptr;
    atom->set3D(_x, _y, _z);
    return atom;
}

std::string JMolAdapter::writeAsPDB() {
    return writeAs("pdb");
}

std::string JMolAdapter::writeAs(const std::string &_formatSuffix) {
    checkOBMol();
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

std::string JMolAdapter::writeAsSMI() {
    return writeAs("can");
}

void JMolAdapter::readAsPDB(const std::string &_pdbBuffer) {
    readAs(_pdbBuffer, "pdb");
}

void JMolAdapter::readAsSMI(const std::string &_pdbBuffer) {
    readAs(_pdbBuffer, "smi");
}

void JMolAdapter::readAs(const std::string &_dataBuffer, const std::string &_formatSuffix) {
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

void JMolAdapter::onMolUpdated() {
    is3DInfoLatest = is2DInfoLatest = false;
}

bool JMolAdapter::runForcefield() {
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
    pFF->SetLogLevel(OBFF_LOGLVL_HIGH);
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
    FOR_ATOMS_OF_MOL(obAtomIter, *obMol) {
        auto obAtom = obAtomIter.operator->();
        auto aid=atomIdMap2[obAtom];
        auto&atom=*getAtom(aid);
        atom.set3D(obAtom->x(), obAtom->y(), obAtom->z());
//        std::cout<<obAtom->z()<<std::endl;
    }
//    sync3D();
    return true;
}

void JMolAdapter::syncAtoms(std::function<void(JAtom &, OpenBabel::OBAtom *)> _func) {
    for (auto&[aid, obAtom]:atomIdMap) {
        auto atom = getAtom(aid);
        if (atom) {
            _func(*atom, obAtom);
        }
    }
}

void JMolAdapter::sync3D() {
    syncAtoms([&](JAtom &atom, OpenBabel::OBAtom *obAtom) {
        atom.set3D(obAtom->x(), obAtom->y(), obAtom->z());
    });
    is3DInfoLatest = true;
}

bool JMolAdapter::generate2D() {
    checkOBMol();
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

bool JMolAdapter::generate3D() {
    checkOBMol();
    return runForcefield();
}

void JMolAdapter::syncNewEntityFromOBMol() {
    FOR_ATOMS_OF_MOL(obAtomIter, *obMol) {
        auto obAtom = obAtomIter.operator->();
        auto it = atomIdMap2.find(obAtom);
        if (atomIdMap2.end() != it) { continue; }
        onMolUpdated();
        auto atom = JMol::addAtom(obAtom->GetAtomicNum());
        if (atom) {
            atomIdMap[atom->getId()] = obAtom;
            atomIdMap2[obAtom] = atom->getId();
        }
    }
    FOR_BONDS_OF_MOL(obBondIter, *obMol) {
        auto obBond = obBondIter.operator->();
        auto it = bondIdMap2.find(obBond);
        if (bondIdMap2.end() != it) { continue; }
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
    isOBMolLatest = true;
}

void JMolAdapter::checkOBMol() {
    if (!isOBMolLatest) {
        resetOBMol();
    }
    isOBMolLatest = true;
}

void JMolAdapter::addOBAtom(JAtom &_atom) {
    auto obAtom = obMol->NewAtom();
    obAtom->SetAtomicNum(_atom.getAtomicNumber());
    atomIdMap[_atom.getId()] = obAtom;
    atomIdMap2[obAtom] = _atom.getId();
}

void JMolAdapter::addOBBond(JBond &_bond) {
    auto obBond = obMol->NewBond();
    bondIdMap[_bond.getId()] = obBond;
    bondIdMap2[obBond] = _bond.getId();
    int obBondOrder;
    switch (_bond.getType()) {
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
    auto from = _bond.getFrom();
    if (from) {
        auto obFrom = atomIdMap.find(from->getId());
        if (atomIdMap.end() != obFrom) {
            obBond->SetBegin(obFrom->second);
        }
    }
    auto to = _bond.getTo();
    if (to) {
        auto obTo = atomIdMap.find(to->getId());
        if (atomIdMap.end() != obTo) {
            obBond->SetEnd(obTo->second);
        }
    }
}

void JMolAdapter::onExtraDataNeeded() {
    JMol::onExtraDataNeeded();
    checkOBMol();
}

void JMolAdapter::resetOBMol() {
    std::cout << "reset" << std::endl;
    bondIdMap.clear();
    atomIdMap.clear();
    bondIdMap2.clear();
    atomIdMap2.clear();
    delete obMol;
    obMol = new OpenBabel::OBMol();
    onMolUpdated();
    loopAtomVec([&](JAtom &atom) {
        addOBAtom(atom);
    });
    loopBondVec([&](JBond &bond) {
        addOBBond(bond);
    });
}


