#include "jmol_adapter.h"
#include "jmol_p.h"
#include "math/alkane_graph.h"

#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/ring.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/obiter.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/generic.h>

#include <coordgenlibs/sketcherMinimizer.h>

using namespace v1_1;


JMolAdapter::JMolAdapter() : isOBMolLatest(true), obMol(std::make_shared<OpenBabel::OBMol>()) {
//    std::cerr << __FUNCTION__;
}

JMolAdapter::~JMolAdapter() {
//    std::cerr << __FUNCTION__;
}

JMolAdapter::JMolAdapter(const JMolAdapter &_jMolAdapter) :
        isOBMolLatest(false), obMol(std::make_shared<OpenBabel::OBMol>()), JMol() {
//    std::cerr << __FUNCTION__ << "const&";
    id = _jMolAdapter.id + 1;
    idBase = _jMolAdapter.idBase;
    std::unordered_multimap<id_type, std::pair<float, std::shared_ptr<JAtom>>> saMap;
    const_cast<JMolAdapter &>(_jMolAdapter).loopAtomVec([&](JAtom &_atom) {
        auto atom = std::make_shared<JAtom>(_atom.getId(), ElementType::SA);
        *atom = _atom;
        atom->clearSABonds();
        auto &sa = _atom.getSaBonds();
        for (auto&[offset, bond]: sa) {
            saMap.insert({bond->getId(), {offset, atom}});
        }
        atomMap[atom->getId()] = atom;
    });
    const_cast<JMolAdapter &>(_jMolAdapter).loopBondVec([&](JBond &_bond) {
        auto bond = std::make_shared<JBond>(
                _bond.getId(), atomMap[_bond.getFrom()->getId()], atomMap[_bond.getTo()->getId()],
                _bond.getType(), _bond.getFromOffset(), _bond.getToOffset());
        bondMap[bond->getId()] = bond;
        auto it = saMap.equal_range(bond->getId());
        if (it.first != saMap.end()) {
            for (auto itr = it.first; itr != it.second; ++itr) {
                auto&[offset, atom]=itr->second;
                atom->insertSuperAtomBonds(bond, offset);
            }
        }
    });
}

JMolAdapter::JMolAdapter(JMolAdapter &&_jMolAdapter) {
//    std::cerr << __FUNCTION__ << "&&";
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
            "smi",
            "can",
            "inchi",
            "smiles"
    };
    if (sNo3DWhiteList.end() == sNo3DWhiteList.find(_formatSuffix) && !is3DInfoLatest) {
        if (!generate3D())
            throw std::runtime_error("fail to generate 3d");
    }
    return conv.WriteString(obMol.get(), true);
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
    if (!conv.Read(obMol.get(), &ssm)) {
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
    std::cerr << __FUNCTION__;
    if (obMol->Empty())return true;
    try {
        OpenBabel::OBBuilder builder;
        std::cerr << "builder.Build ret" << builder.Build(*obMol);
        // FIXME: 适配器的逻辑是手动添加氢原子，不能使用 OpenBabel 的 PH 计算机制
        // OpenBabel 的 gen3d 工具调用了这个接口，是因为 gen3d 的输入是文件
//         std::cerr << "obMol->AddHydrogens ret" << obMol->AddHydrogens(false, true);
    } catch (...) {
        return false;
    }
    auto pFF = OpenBabel::OBForceField::FindForceField("uff");
    if (!pFF) {
        return false;
    }
    pFF->SetLogLevel(OBFF_LOGLVL_NONE);
    if (!pFF->Setup(*obMol)) {
        std::cerr << "pFF->Setup ret false";
    }
    try {
        pFF->SteepestDescent(100, 1.0e-4);
        pFF->WeightedRotorSearch(50, 50);
        pFF->SteepestDescent(100, 1.0e-6);
        std::cerr << "pFF->UpdateCoordinates ret" << pFF->UpdateCoordinates(*obMol);
    } catch (...) {
        return false;
    }
    sync3D();
    return true;
}

void JMolAdapter::syncAtoms(std::function<void(JAtom &, OpenBabel::OBAtom *)> _func) {
    for (auto&[aid, obAtomId]: atomIdMap) {
        auto atom = getAtom(aid);
        auto obAtom = obMol->GetAtomById(obAtomId);
        if (atom) {
            _func(*atom, obAtom);
        }
    }
}

void JMolAdapter::syncBonds(std::function<void(JBond &, OpenBabel::OBBond *)> _func) {
    for (auto&[bid, obBondId]: bondIdMap) {
        auto bond = getBond(bid);
        auto obBond = obMol->GetBondById(obBondId);
        if (bond) {
            _func(*bond, obBond);
        }
    }
}

void JMolAdapter::sync3D() {
    std::cerr << __FUNCTION__;
    syncAtoms([](JAtom &atom, OpenBabel::OBAtom *obAtom) {
        atom.set3D(obAtom->x(), obAtom->y(), obAtom->z());
    });
    is3DInfoLatest = true;
}

bool JMolAdapter::generate2D() {
    std::cerr << __FUNCTION__;
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
        for (auto&[atom, cAtom]: tmpAtomMap) {
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
    std::cerr << __FUNCTION__;
    checkOBMol();
    return runForcefield();
}

void JMolAdapter::syncNewEntityFromOBMol() {
//    std::cerr << __FUNCTION__;
    onMolUpdated();
    FOR_ATOMS_OF_MOL(obAtomIter, *obMol) {
        auto obAtom = obAtomIter->GetId();
        auto it = atomIdMap2.find(obAtom);
        if (atomIdMap2.end() != it) { continue; }
        auto atom = JMol::addAtom(obAtomIter->GetAtomicNum());
        if (atom) {
            atomIdMap[atom->getId()] = obAtom;
            atomIdMap2[obAtom] = atom->getId();
        }
        atom->setCharge(obAtomIter->GetFormalCharge());
    }
    FOR_BONDS_OF_MOL(obBondIter, *obMol) {
        auto it = bondIdMap2.find(obBondIter->GetId());
        if (bondIdMap2.end() != it) { continue; }
        auto bond = JMol::addBond(getAtom(atomIdMap2[obBondIter->GetBeginAtom()->GetId()]),
                                  getAtom(atomIdMap2[obBondIter->GetEndAtom()->GetId()]));
        if (bond) {
            if (5 == obBondIter->GetBondOrder()) {
                bond->setType(BondType::DelocalizedBond);
            } else if (obBondIter->IsHash()) {
                bond->setType(BondType::DashWedgeBond);
            } else if (obBondIter->IsWedge()) {
                bond->setType(BondType::SolidWedgeBond);
            } else if (obBondIter->IsWedgeOrHash()) {
                bond->setType(BondType::WaveBond);
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
    OpenBabel::OBAtom obAtom;
    if (ElementType::SA == _atom.getType()) {
        // FIXME: 选择砹元素作为字符串的代理，这个元素的特点是价态足够、且一般没人写
        // FIXME: 这样在 3D 渲染的时候，如果没有展开字符串，那么字符串会被显示为一个球
        obAtom.SetAtomicNum(85);
//        auto label = new OpenBabel::OBPairData();
//        label->SetAttribute("UserLabel");
//        label->SetValue(_atom.getName());
//        obAtom.SetData(label);
    } else if (ElementType::H == _atom.getType()) {
        obAtom.SetAtomicNum(1);
    } else {
//        std::cerr << "_atom.getAtomicNumber()=" << _atom.getAtomicNumber();
        obAtom.SetAtomicNum(_atom.getAtomicNumber());

    }
    obAtom.SetFormalCharge(_atom.getCharge());
    obAtom.SetId(obMol->NumAtoms());
    obMol->AddAtom(obAtom, false);

//    if (ElementType::H == _atom.getType()) {
//        obMol->GetAtomById(obAtom.GetId())->SetAtomicNum(1);
//    }
    atomIdMap[_atom.getId()] = obAtom.GetId();
    atomIdMap2[obAtom.GetId()] = _atom.getId();
//    std::cerr << obAtom->GetAtomicMass();
}

void JMolAdapter::addOBBond(JBond &_bond) {
//    auto obBond = obMol->NewBond();
    OpenBabel::OBBond obBond;
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
            obBond.SetAromatic(true);
            obBondOrder = 5;
            break;
        case BondType::SolidWedgeBond:
            obBond.SetWedge(true);
            obBondOrder = 1;
            break;
        case BondType::DashWedgeBond:
            obBond.SetHash(true);
            obBondOrder = 1;
            break;
        case BondType::WaveBond:
            obBond.SetWedgeOrHash(true);
            obBondOrder = 1;
            break;
        default:
            throw std::runtime_error("BondType obTo OpenBabel::BondOrder not Implemented!");
    }
    auto from = _bond.getFrom();
    if (from) {
        auto obFrom = atomIdMap.find(from->getId());
        if (atomIdMap.end() != obFrom) {
            obBond.SetBegin(obMol->GetAtomById(obFrom->second));
        } else {
            throw std::runtime_error("atomIdMap empty for jatom from in addOBBond");
        }
    } else {
        throw std::runtime_error("jmol bond from empty");
    }
    auto to = _bond.getTo();
    if (to) {
        auto obTo = atomIdMap.find(to->getId());
        if (atomIdMap.end() != obTo) {
//            std::cerr << "setEnd" << obMol->GetAtomById(obTo->second)->GetId();
            obBond.SetEnd(obMol->GetAtomById(obTo->second));
        } else {
            throw std::runtime_error("atomIdMap empty for jatom to in addOBBond");
        }
    } else {
        throw std::runtime_error("jmol bond to empty");
    }
    obBond.SetBondOrder(obBondOrder);
    obBond.SetId(obMol->NumBonds());
    obMol->AddBond(obBond);
    bondIdMap[_bond.getId()] = obBond.GetId();
    bondIdMap2[obBond.GetId()] = _bond.getId();
}

void JMolAdapter::onExtraDataNeeded() {
    JMol::onExtraDataNeeded();
    checkOBMol();
}

void JMolAdapter::resetOBMol() {
//    std::cerr << __FUNCTION__;
    bondIdMap.clear();
    atomIdMap.clear();
    bondIdMap2.clear();
    atomIdMap2.clear();
    obMol = std::make_shared<OpenBabel::OBMol>();
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
//    std::cerr << __FUNCTION__;
    onMolUpdated();
    checkOBMol();
    auto atom = JMol::addSuperAtom(_name, _x0, _y0, _x1, _y1);
    if (!atom)return nullptr;
    addOBAtom(*atom);
    return atom;
}

void JMolAdapter::display() {
    JMol::display();
    std::cerr << "ob atom info:";
    syncAtoms([](JAtom &atom, OpenBabel::OBAtom *obAtom) {
        std::cerr << obAtom->GetId() << obAtom->GetIdx() << obAtom->GetAtomicNum();
    });
    std::cerr << "ob bond info:";
    syncBonds([](JBond &bond, OpenBabel::OBBond *obBond) {
        std::cerr << bond.getId();
        std::cerr << obBond->GetBeginAtom()->GetId() << obBond->GetEndAtom()->GetId() << obBond->GetBondOrder();
    });
}


std::shared_ptr<JMol> JMolAdapter::deepClone() const {
    auto newMol = std::make_shared<JMolAdapter>(*this);
    return newMol;
}

std::vector<std::vector<id_type>> JMolAdapter::getLSSR() {
    checkOBMol();
    const auto &obRingVec = obMol->GetLSSR();
    decltype(getLSSR()) result(obRingVec.size());
    for (size_t i = 0; i < obRingVec.size(); i++) {
        auto &ring = obRingVec[i];
        result[i].resize(ring->_path.size());
        for (size_t j = 0; j < ring->_path.size(); j++) {
            result[i][j] = atomIdMap2[ring->_path[j]];
        }
    }
    return result;
}

std::vector<std::vector<id_type>> JMolAdapter::getSSSR() {
    checkOBMol();
    const auto &obRingVec = obMol->GetSSSR();
    decltype(getSSSR()) result(obRingVec.size());
    for (size_t i = 0; i < obRingVec.size(); i++) {
        auto &ring = obRingVec[i];
        result[i].resize(ring->_path.size());
        for (size_t j = 0; j < ring->_path.size(); j++) {
            result[i][j] = atomIdMap2[obMol->GetAtom(ring->_path[j])->GetId()];
        }
    }
    return result;
}

void JMolAdapter::exceedAllData() {
    JMol::exceedAllData();
    bondIdMap.clear();
    atomIdMap.clear();
    bondIdMap2.clear();
    atomIdMap2.clear();
    obMol = std::make_shared<OpenBabel::OBMol>();
    onMolUpdated();
}

void JMolAdapter::rebuildAllData() {
    resetOBMol();
}

std::vector<std::string> GetWritableFormats() {
    std::vector<std::string> result;
    OpenBabel::OBPlugin::ListAsVector("formats", nullptr, result);
    return result;
}


std::vector<std::shared_ptr<JMol>> JMolAdapter::split() {
    AlkaneGraph<size_t> graph;
    std::unordered_map<id_type, size_t> j2gAidMap, g2jMap;
    std::vector<std::shared_ptr<JMol>> result;
    size_t gAidIdx = 0;
    loopAtomVec([&](JAtom &atom) {
        j2gAidMap[atom.getId()] = gAidIdx++;
    });
    loopBondVec([&](JBond &bond) {
        auto from = bond.getFrom(), to = bond.getTo();
        if (from && to) {
            graph.push_back(j2gAidMap[from->getId()], j2gAidMap[to->getId()]);
        }
    });
    for (auto&[jAid, gAid]: j2gAidMap) { g2jMap[gAid] = jAid; }
    // 保证一个连通片不和其它连通片共享边 or 节点
    std::vector<std::unordered_set<size_t>> groups = graph.dfsTraceGroup();
//    for (auto &group:groups) {
//        auto mol = std::make_shared<JMolAdapter>();
//        std::unordered_map<size_t, size_t> aidMap;//<old,new>
//        for (auto &gid:group) {
//            size_t oldAid = g2jMap[gid];
//            auto atom = _mol.getAtomById(oldAid);
//            if (!atom) continue;
//            auto newAtom = mol->addAtom(atom->getAtomicNumber());
//            aidMap[oldAid] = newAtom->getId();
//        }
//        _mol.safeTraverseBonds([&](const size_t &bid) {
//            auto bond = _mol.getBondById(bid);
//            if (!bond)return;
//            if (notExist(group, bond->getAtomFrom())) return;
//            mol->addBond(aidMap[bond->getAtomFrom()], aidMap[bond->getAtomTo()],
//                         bond->getBondType());
//        });
//        result.push_back(std::move(mol));
//    }
    return result;
}

