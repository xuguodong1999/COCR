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

std::shared_ptr<JBond> JMolAdapter::addBond(
        std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2, const BondType &_type,
        const float &_offset1, const float &_offset2) {
    onMolUpdated();
    checkOBMol();
    auto bond = JMol::addBond(_a1, _a2, _type, _offset1, _offset2);
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
    // TODO: 一些不需要运行分子力场的非3D坐标格式
    static std::unordered_set<std::string> sNo3DWhiteList = {
//            "smi",
//            "can"
    };
    if (sNo3DWhiteList.end() == sNo3DWhiteList.find(_formatSuffix) && !is3DInfoLatest) {
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
    qDebug() << __FUNCTION__;
    if (obMol->Empty())return true;
    try {
        OpenBabel::OBBuilder builder;
        builder.Build(*obMol);
//        obMol->CorrectForPH();
        obMol->AddHydrogens(false, true);
    } catch (...) {
        return false;
    }
    if (obMol->NumAtoms() > 30) {// 分子力场太慢了……只给小分子优化，大分子只用几何构造
        sync3D();
        return true;
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
        pFF->SteepestDescent(200, 1.0e-4);
        pFF->WeightedRotorSearch(20, 200);
        pFF->SteepestDescent(200, 1.0e-6);
        pFF->UpdateCoordinates(*obMol);
    } catch (...) {
        return false;
    }
    sync3D();
    return true;
}

void JMolAdapter::syncAtoms(std::function<void(JAtom &, OpenBabel::OBAtom *)> _func) {
    for (auto&[aid, obAtomId]:atomIdMap) {
        auto atom = getAtom(aid);
        auto obAtom = obMol->GetAtomById(obAtomId);
        if (atom) {
            _func(*atom, obAtom);
        }
    }
}

void JMolAdapter::sync3D() {
    qDebug() << __FUNCTION__;
    syncAtoms([](JAtom &atom, OpenBabel::OBAtom *obAtom) {
        atom.set3D(obAtom->x(), obAtom->y(), obAtom->z());
    });
    is3DInfoLatest = true;
}

bool JMolAdapter::generate2D() {
    qDebug() << __FUNCTION__;
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
    qDebug() << __FUNCTION__;
    checkOBMol();
    return runForcefield();
}

void JMolAdapter::syncNewEntityFromOBMol() {
    qDebug() << __FUNCTION__;
    FOR_ATOMS_OF_MOL(obAtomIter, *obMol) {
        auto obAtom = obAtomIter->GetId();
        auto it = atomIdMap2.find(obAtom);
        if (atomIdMap2.end() != it) { continue; }
        onMolUpdated();
        auto atom = JMol::addAtom(obAtomIter->GetAtomicNum());
        if (atom) {
            atomIdMap[atom->getId()] = obAtom;
            atomIdMap2[obAtom] = atom->getId();
        }
    }
    FOR_BONDS_OF_MOL(obBondIter, *obMol) {
        auto it = bondIdMap2.find(obBondIter->GetId());
        if (bondIdMap2.end() != it) { continue; }
        onMolUpdated();
        auto bond = JMol::addBond(getAtom(atomIdMap2[obBondIter->GetBeginAtom()->GetId()]),
                                  getAtom(atomIdMap2[obBondIter->GetEndAtom()->GetId()]));
        if (bond) {
            if (5 == obBondIter->GetBondOrder()) {
                bond->setType(BondType::DelocalizedBond);
            } else if (obBondIter->IsHash()) {
                bond->setType(BondType::DownBond);
            } else if (obBondIter->IsWedge()) {
                bond->setType(BondType::UpBond);
            } else if (obBondIter->IsWedgeOrHash()) {
                bond->setType(BondType::ImplicitBond);
            } else {
                bond->setOrder(obBondIter->GetBondOrder());
            }
            bondIdMap[bond->getId()] = obBondIter->GetId();
            bondIdMap2[obBondIter->GetId()] = bond->getId();
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
    if (ElementType::SA != _atom.getType()) {
        obAtom->SetAtomicNum(_atom.getAtomicNumber());
    } else {
        // FIXME: 选择砹元素作为字符串的代理，这个元素的特点是价态足够、且一般没人写
        // FIXME: 这样在 3D 渲染的时候，如果没有展开字符串，那么字符串会被显示为一个球
        obAtom->SetAtomicNum(85);
    }
    atomIdMap[_atom.getId()] = obAtom->GetId();
    atomIdMap2[obAtom->GetId()] = _atom.getId();
}

void JMolAdapter::addOBBond(JBond &_bond) {
    auto obBond = obMol->NewBond();
    bondIdMap[_bond.getId()] = obBond->GetId();
    bondIdMap2[obBond->GetId()] = _bond.getId();
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
            obBond->SetBegin(obMol->GetAtomById(obFrom->second));
        }
    }
    auto to = _bond.getTo();
    if (to) {
        auto obTo = atomIdMap.find(to->getId());
        if (atomIdMap.end() != obTo) {
            obBond->SetEnd(obMol->GetAtomById(obTo->second));
        }
    }
}

void JMolAdapter::onExtraDataNeeded() {
    JMol::onExtraDataNeeded();
    checkOBMol();
}

void JMolAdapter::resetOBMol() {
    qDebug() << __FUNCTION__;
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

std::shared_ptr<JAtom>
JMolAdapter::addSuperAtom(const std::string &_name, const float &_x0, const float &_y0, const float &_x1,
                          const float &_y1) {
    onMolUpdated();
    checkOBMol();
    auto atom = JMol::addSuperAtom(_name, _x0, _y0, _x1, _y1);
    if (!atom)return nullptr;
    addOBAtom(*atom);
    return atom;
}

