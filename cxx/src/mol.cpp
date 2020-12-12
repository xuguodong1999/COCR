#ifdef __ANDROID__
#define USE_OPENBABEL
#elif defined(__linux__)
#define USE_COORDGEN2D
#define USE_OPENBABEL
#define USE_RDKIT
#endif

#ifdef _WIN32
#define USE_COORDGEN2D
#define USE_RDKIT
#endif

#include "mol.hpp"
#include "std_util.hpp"
#include "opencv_util.hpp"
#include "alkane_graph.hpp"

#ifdef USE_COORDGEN2D

#include <coordgen/sketcherMinimizer.h>

#endif

#ifdef USE_RDKIT

#include <GraphMol/Chirality.h>
#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/AtomIterators.h>
#include <GraphMol/Depictor/RDDepictor.h>
#include <GraphMol/MolAlign/AlignMolecules.h>
#include <GraphMol/DistGeomHelpers/Embedder.h>
#include <GraphMol/ForceFieldHelpers/UFF/UFF.h>
#include <GraphMol/ForceFieldHelpers/MMFF/MMFF.h>
#include <GraphMol/MolOps.h>

#endif

#ifdef USE_OPENBABEL

#include <openbabel/mol.h>
#include <openbabel/bond.h>
#include <openbabel/obiter.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>

#endif

#include <random>

using namespace std;

#ifdef USE_RDKIT

inline JBondType convertRDKitBondTypeToJBondType(
        const RDKit::Bond::BondType &_stereo, const RDKit::Bond::BondDir &_dir) {
    if (RDKit::Bond::BondType::SINGLE == _stereo) {
        if (RDKit::Bond::BondDir::NONE == _dir) {
            return JBondType::SingleBond;
        } else if (RDKit::Bond::BondDir::BEGINWEDGE == _dir) {
            return JBondType::SolidWedgeBond;
        } else if (RDKit::Bond::BondDir::BEGINDASH == _dir) {
            return JBondType::DashWedgeBond;
        } else if (RDKit::Bond::BondDir::UNKNOWN == _dir) {
            return JBondType::WaveBond;
        } else {
            std::cerr << "convert dir " << static_cast<int>(_dir)
                      << " To JBondType failed " << std::endl;
            exit(-1);
        }
    } else if (RDKit::Bond::BondType::DOUBLE == _stereo) {
        return JBondType::DoubleBond;
    } else if (RDKit::Bond::BondType::TRIPLE == _stereo) {
        return JBondType::TripleBond;
    } else if (RDKit::Bond::BondType::AROMATIC == _stereo) {
        return JBondType::DelocalizedBond;
    } else {
        std::cerr << "convert stereo " << static_cast<int>(_stereo)
                  << " To JBondType failed " << std::endl;
        exit(-1);
    }
}

inline std::pair<RDKit::Bond::BondType, RDKit::Bond::BondDir>
convertJBondTypeToRDKitBondType(const JBondType &_bondType) {
    RDKit::Bond::BondType bondType;
    RDKit::Bond::BondDir bondDir = RDKit::Bond::BondDir::NONE;
    switch (_bondType) {
        case JBondType::SolidWedgeBond:
            bondDir = RDKit::Bond::BondDir::BEGINWEDGE;
        case JBondType::DashWedgeBond:
            bondDir = RDKit::Bond::BondDir::BEGINDASH;
        case JBondType::WaveBond:
            bondDir = RDKit::Bond::BondDir::UNKNOWN;
        case JBondType::SingleBond :
            bondType = RDKit::Bond::BondType::SINGLE;
            break;
        case JBondType::DoubleBond:
            bondType = RDKit::Bond::BondType::DOUBLE;
            break;
        case JBondType::TripleBond:
            bondType = RDKit::Bond::BondType::TRIPLE;
            break;
        case JBondType::DelocalizedBond:
            bondType = RDKit::Bond::BondType::AROMATIC;
            break;
        default: {
            std::cerr << "convert " << static_cast<int>(_bondType)
                      << " To RDKitBondType failed " << std::endl;
            exit(-1);
        }
    }
    return {bondType, bondDir};
}

std::pair<std::shared_ptr<RDKit::RWMol>, std::unordered_map<size_t, unsigned int>>
convertJMolToRWMol(const JMol &_jmol) {
    auto rwMol = std::make_shared<RDKit::RWMol>();
    std::unordered_map<size_t, unsigned int> j2rAidMap;
    for (auto&[jAid, jAtom]:_jmol.getAtomsMap()) {
        auto atom = std::make_shared<RDKit::Atom>(jAtom->getAtomicNumber());
        auto aid = rwMol->addAtom(atom.get(), true, false);
        j2rAidMap.insert({jAid, aid});
    }
    for (auto&[jBid, jBond]:_jmol.getBondsMap()) {
        auto[stereo, dir]=convertJBondTypeToRDKitBondType(jBond->getBondType());
        rwMol->addBond(j2rAidMap[jBond->getAtomFrom()],
                       j2rAidMap[jBond->getAtomTo()],
                       stereo);
        rwMol->getBondBetweenAtoms(j2rAidMap[jBond->getAtomFrom()],
                                   j2rAidMap[jBond->getAtomTo()])->setBondDir(dir);
    }
    return {std::move(rwMol), std::move(j2rAidMap)};
}

#endif

// FIXME: RDKit::Atom::ChiralType 没有传递到 RDKit::Bond::BondDir
const std::unordered_map<size_t, std::shared_ptr<JBond>> &JMol::getBondsMap() const {
    return bondsMap;
}

const std::unordered_map<size_t, std::shared_ptr<JAtom>> &JMol::getAtomsMap() const {
    return atomsMap;
}

std::shared_ptr<JAtom> JMol::addAtom(const size_t &_atomicNumber) {
    auto aid = mAids++;
    auto atom = std::make_shared<JAtom>(aid, _atomicNumber);
    atomsMap.insert({aid, atom});
    return std::move(atom);
}

std::shared_ptr<JBond> JMol::addBond(const size_t &_atomFromId, const size_t &_atomToId,
                                     const JBondType &_bondType) {
    auto bid = mBids++;
    auto bond = std::make_shared<JBond>(bid, _atomFromId, _atomToId, _bondType);
    bondsMap.insert({bid, bond});
    return std::move(bond);
}

JMol::JMol() {
    mAids = mBids = std::numeric_limits<size_t>::lowest();
}

void JMol::clear() {
    bondsMap.clear();
    atomsMap.clear();
    atomValenceMap.clear();
    neighborAtomsMap.clear();
    neighborBondsMap.clear();
    aromaticRingAids.clear();
    aromaticRingBids.clear();
    atomPosMap2D.clear();
    atomPosMap3D.clear();
    mAids = mBids = std::numeric_limits<size_t>::lowest();
}

void JMol::set(const string &_smiles) {
    clear();
#ifdef USE_RDKIT
    // the caller is responsible for freeing this.
    auto rwMol = RDKit::SmilesToMol(_smiles, 0, true);
    std::map<unsigned int, size_t> r2jAidMap;
//    for (auto it = rwMol->beginAtoms(); it != rwMol->endAtoms(); it++) {
    for (unsigned int i = 0; i < rwMol->getNumAtoms(); i++) {
        const auto rAtom = rwMol->getAtomWithIdx(i);
        auto jAtom = addAtom(rAtom->getAtomicNum());
        r2jAidMap.insert({rAtom->getIdx(), jAtom->getId()});
    }
    for (unsigned int i = 0; i < rwMol->getNumBonds(true); i++) {
        const auto rBond = rwMol->getBondWithIdx(i);
        auto jBond = addBond(r2jAidMap[rBond->getBeginAtomIdx()],
                             r2jAidMap[rBond->getEndAtomIdx()]);
        jBond->setType(convertRDKitBondTypeToJBondType(
                rBond->getBondType(), rBond->getBondDir()));
    }
    delete rwMol;
#else
    std::cerr << "error: define \"USE_RDKIT\" in mol.cpp to enable JMol::set" << std::endl;
    exit(-1);
#endif
}

const float &JMol::getFontSize() const {
    return fontSize;
}

std::unordered_map<size_t, cv::Point2f> JMol::get2DCoordinates() const {
    std::unordered_map<size_t, cv::Point2f> depAtomPosMap;
#ifdef USE_COORDGEN2D
    // 踩坑系列：sketcherMinimizer 析构的时候会 delete 所有关联的指针
    // 这里 sketcherMinimizerMolecule 一定得 new 一个，用智能指针或值就会报 double free
    // 我吃了一个苹果，苹果没了，我也没了
    sketcherMinimizer minimizer;
    auto sMol = new sketcherMinimizerMolecule();
    std::unordered_map<size_t, sketcherMinimizerAtom *> j2cAtomMap;
    for (auto&[id, atom]:atomsMap) {
        auto a = sMol->addNewAtom();
        a->setAtomicNumber(atom->getAtomicNumber());
        j2cAtomMap[id] = a;
    }
    std::vector<sketcherMinimizerBond *> cBondVec;
    for (auto&[id, bond]:bondsMap) {
        auto b = sMol->addNewBond(j2cAtomMap[bond->getAtomFrom()],
                                  j2cAtomMap[bond->getAtomTo()]);
        b->setBondOrder(std::lround(bond->asValence().floatValue()));
        cBondVec.push_back(b);
    }
    minimizer.initialize(sMol);
    minimizer.runGenerateCoordinates();
    for (auto&[aid, cAtom]:j2cAtomMap) {
        auto pos = cAtom->getCoordinates();
        depAtomPosMap[aid] = cv::Point2f(pos.x(), pos.y());
    }
#elif defined(USE_RDKIT)
    // FIXME: RDKit 内置2d引擎画的三键有问题
    auto[rwMol, j2rAidMap] = convertJMolToRWMol(*this);
    std::unordered_map<unsigned int, size_t> r2jAidMap;
    for (auto&[jAid, rAid]:j2rAidMap) {
        r2jAidMap[rAid] = jAid;
    }
    j2rAidMap.clear();
    auto roMol = std::make_shared<RDKit::ROMol>(*(rwMol.get()));
//    RDDepict::compute2DCoords(*(roMol.get()));
    RDDepict::compute2DCoordsMimicDistMat(*(roMol.get()));
    auto conf = roMol->getConformer();
    for (unsigned int i = 0; i < rwMol->getNumAtoms(); i++) {
        auto pos = conf.getAtomPos(i);
        depAtomPosMap[r2jAidMap[i]] = cv::Point2f(pos.x, pos.y);
    }
#else
    std::cerr << "no coordinate engine available in mol.cpp" << std::endl;
    exit(-1);
#endif
    // FIXME: 想一下怎么设计函数的 const
    float &_fontSize = const_cast<float &>(fontSize);
    _fontSize = 0;
    for (auto&[_, bond]:bondsMap) {
        _fontSize += getDistance2D(depAtomPosMap[bond->getAtomFrom()],
                                   depAtomPosMap[bond->getAtomTo()]);
    }
    if (bondsMap.empty()) {
        _fontSize = 30;
    } else {
        _fontSize /= bondsMap.size();
    }
    return std::move(depAtomPosMap);
}

void JMol::randomize(const float &_addHydrogenProb, bool _replaceBond, bool _replaceAtom,
                     bool _addAromaticRing, bool _addCommonRing) {
    updateAtomValenceMap();
    // 换化学键类型
    auto replaceBondType = [&](const size_t &_bid) {
        const auto &bond = bondsMap[_bid];
        const size_t &from = bond->getAtomFrom(), to = bond->getAtomTo();
        const auto &fromAtom = atomsMap[from];
        const auto &toAtom = atomsMap[to];
        const frac deltaFrom = ElementValenceData[fromAtom->getElementType()]
                               - atomValenceMap[from];
        const frac deltaTo = ElementValenceData[toAtom->getElementType()]
                             - atomValenceMap[to];
        if (deltaFrom >= 1 && deltaTo >= 1) {
            frac delta = std::min(deltaFrom, deltaTo);
            std::vector<JBondType> sBondCandidates;
            if (delta >= 2) {// 三键、双键
                sBondCandidates = {JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::DoubleBond, JBondType::TripleBond};
            } else {// 双键
                sBondCandidates = {JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::DoubleBond};
            }
            // 检查键端原子是不是已经有双键
            bool allowReplace = true;
            for (auto&[id2, bond2]:bondsMap) {
                if (_bid == id2) {
                    continue;
                }
                if (from == bond2->getAtomFrom() || from == bond2->getAtomTo()) {
                    if (JBondType::DoubleBond == bond2->getBondType()) {
                        allowReplace = false;
                        break;
                    }
                }
                if (to == bond2->getAtomFrom() || to == bond2->getAtomTo()) {
                    if (JBondType::DoubleBond == bond2->getBondType()) {
                        allowReplace = false;
                        break;
                    }
                }
            }
            if (allowReplace) {
                const frac oldValence = bond->asValence();
                bond->setType(randSelect(sBondCandidates));
                atomValenceMap[from] += (bond->asValence() - oldValence);
                atomValenceMap[to] += (bond->asValence() - oldValence);
            }
        }
    };
    if (_replaceBond)
        safeTraverseBonds(replaceBondType);
    // 换原子类型
    if (_replaceAtom) {
        static std::vector<ElementType> sAtomCandidates = {
                ElementType::H, ElementType::C,
                ElementType::N, ElementType::P, ElementType::B,
                ElementType::O, ElementType::S,
                ElementType::F, ElementType::Cl, ElementType::Br, ElementType::I
        };
        for (auto&[id, atom]:atomsMap) {
            const auto &ele = randSelect(sAtomCandidates);
            if (atomValenceMap[id] <= ElementValenceData[ele]) {
                atom->setElementType(ele);
            }
        }
    }
    // 添加官能团
    auto add_group = [&](const size_t &_aid) {// COOH CHO etc
        const auto &atom = atomsMap[_aid];
        if (atomValenceMap[_aid] == 1 && ElementType::C == atom->getElementType()) {
            addGroup(_aid);
        }
    };
    safeTraverseAtoms(add_group);
    // 添加杂环
    std::unordered_set<size_t> usedAids;
    auto replaceBondWithRing = [&](const size_t &_bid) {
        const auto &bond = bondsMap[_bid];
        if (byProb(0.2) && JBondType::SingleBond == bond->getBondType()) {
            size_t from = bondsMap[_bid]->getAtomFrom(), to = bondsMap[_bid]->getAtomTo();
            // 避免一个原子挂上好几个环导致排版引擎崩溃
            if (notExist(usedAids, from) && notExist(usedAids, from)) {
                if (_addAromaticRing && byProb(0.5)) {
                    addAromaticRing(_bid);
                } else if (_addCommonRing) {
                    addCommonRing(_bid);
                }
            }
            usedAids.insert(from);
            usedAids.insert(to);
        }
    };
    if (_addAromaticRing || _addCommonRing)
        safeTraverseBonds(replaceBondWithRing);
    // 打补丁：删掉 P#C B#C
    auto clearStrangeBond = [&](const size_t &_bid) {
        const auto &bond = bondsMap[_bid];
        if (JBondType::TripleBond == bond->getBondType()) {
            auto ele = atomsMap[bond->getAtomFrom()]->getElementType();
            if (ElementType::C != ele && ElementType::N != ele) {
                atomsMap[bond->getAtomFrom()]->setElementType(ElementType::C);
            }
            ele = atomsMap[bond->getAtomTo()]->getElementType();
            if (ElementType::C != ele && ElementType::N != ele) {
                atomsMap[bond->getAtomTo()]->setElementType(ElementType::C);
            }
        }
    };
    safeTraverseBonds(clearStrangeBond);
    // 添加氢原子
    auto add_hydrogen = [&](const size_t &_aid) {
        const auto &atom = atomsMap[_aid];
        if (ElementType::C != atom->getElementType()) {// 杂原子较大可能加氢
            if (byProb(_addHydrogenProb + 0.5)) {
                addHs(_aid);
            }
        } else if (byProb(_addHydrogenProb)) {// 碳原子按概率加氢
            addHs(_aid);
        }
    };
    safeTraverseAtoms(add_hydrogen);
    // 楔形键
    updateNeighborInfoMap();
    std::unordered_map<size_t, bool> allowWedge;
    for (auto&[id, _]:atomsMap) {
        allowWedge[id] = true;
    }
    for (auto&[aid, atom]:atomsMap) {
        bool allowThisWedge = true;
        if (neighborAtomsMap[aid].size() == 4 && ElementType::C == atom->getElementType()) {
            for (auto &nid:neighborAtomsMap[aid]) {
                if (!allowWedge[nid]) {
                    allowThisWedge = false;
                    break;
                }
            }
            if (allowThisWedge) {
                allowWedge[aid] = false;
                std::vector<std::shared_ptr<JBond>> bonds;
                for (auto &bid:neighborBondsMap[aid]) {
                    bonds.push_back(bondsMap[bid]);
                }
                // 优先为 C-H 键加构型
                int okNeighbor = 0;
                for (auto &bond:bonds) {
                    if (okNeighbor == 2) {
                        break;
                    }
                    if (ElementType::H == atomsMap[bond->getAtomFrom()]->getElementType() ||
                        ElementType::H == atomsMap[bond->getAtomTo()]->getElementType()) {
                        if (bond->setFrom(aid)) {
                            if (okNeighbor == 0)
                                bond->setType(JBondType::SolidWedgeBond);
                            else
                                bond->setType(JBondType::DashWedgeBond);
                            okNeighbor += 1;
                        }

                    }
                }
                while (okNeighbor < 2) {
                    auto &bond = randSelect(bonds);
                    if (bond->setFrom(aid)) {
                        if (okNeighbor == 0)
                            bond->setType(JBondType::SolidWedgeBond);
                        else
                            bond->setType(JBondType::DashWedgeBond);
                        okNeighbor += 1;
                    }
                }
            }
        }
    }
    // TODO: 波浪线
    // TODO: 添加不展开的字符串
}

void JMol::addHs(const size_t &_aid) {
    auto &valence = atomValenceMap.at(_aid);
    const auto &atom = atomsMap[_aid];
    frac numOfH = ElementValenceData[atom->getElementType()] - valence;
    if (numOfH >= 1) {
        int hsNum = std::lround(numOfH.floatValue());
        for (int i = 0; i < hsNum; i++) {
            auto h = addAtom(1);
            const size_t &hid = h->getId();
            addBond(hid, _aid, JBondType::SingleBond);
            atomValenceMap[hid] = 1;
        }
        valence += numOfH;
    }
}

std::string JMol::toSMILES(bool _addRandomStereo) const {
#ifdef USE_RDKIT
    auto[rwMol, _] = convertJMolToRWMol(*this);
#ifdef WIN32
    if (_addRandomStereo) {
        static std::vector<RDKit::Atom::ChiralType> candidates = {
                RDKit::Atom::ChiralType::CHI_TETRAHEDRAL_CW,
                RDKit::Atom::ChiralType::CHI_TETRAHEDRAL_CCW
        };
        for (unsigned int i = 0; i < rwMol->getNumAtoms(); i++) {
            rwMol->getAtomWithIdx(i)->calcImplicitValence(true);
        }
        auto stereos = RDKit::Chirality::findPotentialStereo((*rwMol.get()), false);
        for (auto &stereo:stereos) {
            if (stereo.centeredOn != RDKit::Chirality::StereoInfo::NOATOM) {
                rwMol->getAtomWithIdx(stereo.centeredOn)->
                        setChiralTag(randSelect(candidates));
            }
        }
    }
#endif
    auto roMol = std::make_shared<RDKit::ROMol>(*(rwMol.get()));
    return RDKit::MolToSmiles(
            *(roMol.get()), true, false, -1,
            true, false, false, false);
#else
    std::cerr << "error: define \"USE_RDKIT\" in mol.cpp to enable SMILES function"
              << std::endl;
    return "";
#endif

}

void JMol::updateAtomValenceMap() {
    atomValenceMap.clear();
    auto addValence4Atom = [&](const size_t &_aid, const frac &_valence) {
        if (notExist(atomValenceMap, _aid)) {
            atomValenceMap[_aid] = _valence;
        } else {
            atomValenceMap[_aid] += _valence;
        }
    };
    for (auto&[id, bond]:bondsMap) {
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
    }
    // 处理没有化学键的情况，比如加氢之前的甲烷
    for (auto&[id, atom]:atomsMap) {
        if (notExist(atomValenceMap, id)) {
            atomValenceMap[id] = 0;
        }
    }
}

void JMol::updateAtomValenceMap(const vector<size_t> &_bids) {
    for (auto &bid:_bids) {
        const auto &bond = bondsMap[bid];
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
    }
}

void JMol::addValence4Atom(const size_t &_aid, const frac &_valence) {
    if (notExist(atomValenceMap, _aid)) {
        atomValenceMap[_aid] = _valence;
    } else {
        atomValenceMap[_aid] += _valence;
    }
}

void JMol::addAromaticRing(const size_t &_bid) {
    std::vector<std::shared_ptr<JAtom>> newAtoms;
    std::vector<std::shared_ptr<JBond>> newBonds;
    auto from = bondsMap[_bid]->getAtomFrom(), to = bondsMap[_bid]->getAtomTo();
    removeBond(_bid);
    int index = rand() % 9;
    switch (index) {
        case 0:
        case 1:
        case 2:
        case 3: {// 呋喃系
            std::vector<size_t> atomicNumbers = {7, 8, 15, 16};
            newAtoms = {
                    addAtom(atomicNumbers[index]),
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6)
            };
            std::unordered_set<size_t> atomInRing, bondInRing;
            for (auto &atom:newAtoms) {
                atomInRing.insert(atom->getId());
            }
            aromaticRingAids.push_back({std::move(atomInRing)});
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[0]->getId(), newAtoms[4]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId(),
                            JBondType::DoubleBond)
            };
            for (auto &bond:newBonds) {
                bondInRing.insert(bond->getId());
            }
            aromaticRingBids.push_back({std::move(bondInRing)});
            newBonds.push_back(addBond(from, newAtoms[1 + rand() % 2]->getId(),
                                       JBondType::SingleBond));
            newBonds.push_back(addBond(to, newAtoms[3 + rand() % 2]->getId(),
                                       JBondType::SingleBond));
            break;
        }
        case 4:
        case 5: {// 苯环系
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            newAtoms = {
                    addAtom(6),
                    addAtom(6),
                    addAtom(randSelect(atomicNumbers)),
                    addAtom(randSelect(atomicNumbers)),
                    addAtom(randSelect(atomicNumbers)),
                    addAtom(randSelect(atomicNumbers))
            };
            std::unordered_set<size_t> atomInRing, bondInRing;
            for (auto &atom:newAtoms) {
                atomInRing.insert(atom->getId());
            }
            aromaticRingAids.push_back({std::move(atomInRing)});
            size_t pos1 = newAtoms[0]->getId(), pos2 = newAtoms[1]->getId();
            std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[0]->getId(), newAtoms[5]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[4]->getId(), newAtoms[5]->getId(),
                            JBondType::SingleBond)
            };
            for (auto &bond:newBonds) {
                bondInRing.insert(bond->getId());
            }
            aromaticRingBids.push_back({std::move(bondInRing)});
            newBonds.push_back(addBond(from, pos1, JBondType::SingleBond));
            newBonds.push_back(addBond(to, pos2, JBondType::SingleBond));
            break;
        }
        case 6: {//萘
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            std::vector<size_t> poses;
            const size_t reactSiteNum = 8;
            for (int i = 0; i < reactSiteNum; i++) {
                auto atom = addAtom(randSelect(atomicNumbers));
                newAtoms.push_back(atom);
                if (ElementType::C == atom->getElementType()) {
                    poses.push_back(atom->getId());
                }
            }
            if (poses.size() < 2) {
                size_t pos1 = rand() % (reactSiteNum / 2);
                size_t pos2 = reactSiteNum / 2 + rand() % (reactSiteNum / 2);
                newAtoms[pos1]->setElementType(ElementType::C);
                newAtoms[pos2]->setElementType(ElementType::C);
                poses = {newAtoms[pos1]->getId(), newAtoms[pos2]->getId()};
            } else {
                std::shuffle(poses.begin(), poses.end(), std::default_random_engine());
            }
            // 非反应位点
            for (int i = 0; i < 2; i++) {
                newAtoms.push_back(addAtom(6));
            }
            std::vector<std::pair<size_t, size_t>> sBond = {
                    {2, 3},
                    {4, 10},
                    {5, 6},
                    {7, 8},
                    {9, 10},
                    {1, 9}
            };
            std::vector<std::pair<size_t, size_t>> dBond = {
                    {1, 2},
                    {3, 4},
                    {5, 10},
                    {6, 7},
                    {8, 9}
            };
            for (auto&[a1, a2]:sBond) {
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId()));
            }
            for (auto&[a1, a2]:dBond) {
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId(),
                                           JBondType::DoubleBond));
            }
            // TODO: 把环信息写入 aromaticRingAids 和 aromaticRingBids
            newBonds.push_back(addBond(from, poses[0]));
            newBonds.push_back(addBond(to, poses[1]));
            break;
        }
        case 7: {//蒽
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            std::vector<size_t> poses;
            const size_t reactSiteNum = 10;
            for (int i = 0; i < reactSiteNum; i++) {
                auto atom = addAtom(randSelect(atomicNumbers));
                newAtoms.push_back(atom);
                if (ElementType::C == atom->getElementType()) {
                    poses.push_back(atom->getId());
                }
            }
            if (poses.size() < 2) {
                size_t pos1 = rand() % (reactSiteNum / 2);
                size_t pos2 = reactSiteNum / 2 + rand() % (reactSiteNum / 2);
                newAtoms[pos1]->setElementType(ElementType::C);
                newAtoms[pos2]->setElementType(ElementType::C);
                poses = {newAtoms[pos1]->getId(), newAtoms[pos2]->getId()};
            } else {
                std::shuffle(poses.begin(), poses.end(), std::default_random_engine());
            }
            // 非反应位点
            for (int i = 0; i < 4; i++) {
                newAtoms.push_back(addAtom(6));
            }
            std::vector<std::pair<size_t, size_t>> sBond = {
                    {11, 1},
                    {2,  3},
                    {4,  12},
                    {10, 13},
                    {5,  6},
                    {7,  8},
                    {14, 9},
                    {11, 12},
                    {13, 14}
            };
            std::vector<std::pair<size_t, size_t>> dBond = {
                    {1,  2},
                    {3,  4},
                    {12, 10},
                    {13, 5},
                    {6,  7},
                    {8,  14},
                    {9,  11}
            };
            for (auto&[a1, a2]:sBond) {
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId()));
            }
            for (auto&[a1, a2]:dBond) {
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId(),
                                           JBondType::DoubleBond));
            }
            newBonds.push_back(addBond(from, poses[0]));
            newBonds.push_back(addBond(to, poses[1]));
            break;
        }
        case 8: {//菲
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            std::vector<size_t> poses;
            const size_t reactSiteNum = 10;
            for (int i = 0; i < reactSiteNum; i++) {
                auto atom = addAtom(randSelect(atomicNumbers));
                newAtoms.push_back(atom);
                if (ElementType::C == atom->getElementType()) {
                    poses.push_back(atom->getId());
                }
            }
            if (poses.size() < 2) {
                size_t pos1 = rand() % (reactSiteNum / 2);
                size_t pos2 = reactSiteNum / 2 + rand() % (reactSiteNum / 2);
                newAtoms[pos1]->setElementType(ElementType::C);
                newAtoms[pos2]->setElementType(ElementType::C);
                poses = {newAtoms[pos1]->getId(), newAtoms[pos2]->getId()};
            } else {
                std::shuffle(poses.begin(), poses.end(), std::default_random_engine());
            }
            // 非反应位点
            for (int i = 0; i < 4; i++) {
                newAtoms.push_back(addAtom(6));
            }
            std::vector<std::pair<size_t, size_t>> sBond = {
                    {11, 1},
                    {2,  3},
                    {4,  12},
                    {12, 13},
                    {13, 5},
                    {6,  7},
                    {8,  14},
                    {14, 9},
                    {10, 11}
            };
            std::vector<std::pair<size_t, size_t>> dBond = {
                    {1,  2},
                    {3,  4},
                    {5,  6},
                    {7,  8},
                    {13, 14},
                    {9,  10},
                    {11, 12}
            };
            for (auto&[a1, a2]:sBond) {
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId()));
            }
            for (auto&[a1, a2]:dBond) {
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId(),
                                           JBondType::DoubleBond));
            }
            newBonds.push_back(addBond(from, poses[0]));
            newBonds.push_back(addBond(to, poses[1]));
            break;
        }
        case 9: {
            break;
        }
        case 10: {
            break;
        }
        case 11: {
            break;
        }
        case 12: {
            break;
        }
        default:
            break;
    }
    updateAtomValenceMap(newBonds);
}

void JMol::addCommonRing(const size_t &_bid) {
    std::vector<std::shared_ptr<JAtom>> newAtoms;
    std::vector<std::shared_ptr<JBond>> newBonds;
    auto from = bondsMap[_bid]->getAtomFrom(), to = bondsMap[_bid]->getAtomTo();
    removeBond(_bid);
    std::vector<size_t> nodeNums = {3, 3, 4, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7};
    size_t &nodeNum = randSelect(nodeNums);
    std::vector<size_t> atomicNumbers = {5, 6, 7, 8, 15, 16};
    switch (nodeNum) {
        case 3: {// 三元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 2]->getId()),
                    addBond(to, newAtoms[rand() % 2]->getId())
            };
            break;
        }
        case 4: {// 四元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 3]->getId()),
                    addBond(to, newAtoms[rand() % 3]->getId())
            };
            break;
        }
        case 5: {// 五元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    addBond(newAtoms[4]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 4]->getId()),
                    addBond(to, newAtoms[rand() % 4]->getId())
            };
            break;
        }
        case 6: {// 六元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    addBond(newAtoms[4]->getId(), newAtoms[5]->getId()),
                    addBond(newAtoms[5]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 5]->getId()),
                    addBond(to, newAtoms[rand() % 5]->getId())
            };
            break;
        }
        case 7: {// 七元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    addBond(newAtoms[4]->getId(), newAtoms[5]->getId()),
                    addBond(newAtoms[5]->getId(), newAtoms[6]->getId()),
                    addBond(newAtoms[6]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 6]->getId()),
                    addBond(to, newAtoms[rand() % 6]->getId())
            };
            break;
        }
        case 100: {
            // FIXME: coodgen2d 的桥环渲染有问题
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    addBond(newAtoms[4]->getId(), newAtoms[5]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[5]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[0 + rand() % 3]->getId()),
                    addBond(to, newAtoms[3 + rand() % 3]->getId()),
                    addBond(newAtoms[0]->getId(), newAtoms[6]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[6]->getId())
            };
            break;
        }
        default:
            break;
    }
    updateAtomValenceMap(newBonds);
}

void JMol::removeBond(const size_t &_bid) {
    auto it = bondsMap.find(_bid);
    if (bondsMap.end() != it) {
        auto oldVal = it->second->asValence();
        atomValenceMap[it->second->getAtomFrom()] -= oldVal;
        atomValenceMap[it->second->getAtomTo()] -= oldVal;
        bondsMap.erase(it);
    }
}

void JMol::updateAtomValenceMap(const vector<std::shared_ptr<JBond>> &_bonds) {
    for (auto &bond:_bonds) {
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
    }
}

void JMol::addGroup(const size_t &_aid) {
    std::vector<std::shared_ptr<JAtom>> newAtoms;
    std::vector<std::shared_ptr<JBond>> newBonds;
    int index = rand() % 1;
    switch (index) {
        case 0: {// 羰基、酰基、醛基、羧基、巯基
            static std::vector<size_t> carbonyl = {8, 8, 8, 8, 16};
            static std::vector<size_t> acyl = {1, 8, 9, 16, 17, 53, 35};
            newAtoms = {
                    addAtom(randSelect(carbonyl)),
                    addAtom(randSelect(acyl)),
                    addAtom(6)
            };
            newBonds = {
                    addBond(_aid, newAtoms[2]->getId(),
                            JBondType::SingleBond),// 连羧基碳
                    addBond(newAtoms[2]->getId(), newAtoms[1]->getId(),
                            JBondType::SingleBond),// 连羧基氢
                    addBond(newAtoms[2]->getId(), newAtoms[0]->getId(),
                            JBondType::DoubleBond)// 连羧基氧
            };
            break;
        }
        case 1:
        case 2: {
            break;
        }
        case 3: {
            break;
        }
        default:
            break;
    }
    updateAtomValenceMap(newBonds);
}

void JMol::safeTraverseBonds(const std::function<void(const size_t &)> &func) {
    std::vector<size_t> bids;
    for (auto&[id, _]:bondsMap) {
        bids.push_back(id);
    }
    for (auto &bid:bids) {
        func(bid);
    }
}

void JMol::updateNeighborInfoMap() {
    neighborAtomsMap.clear();
    neighborBondsMap.clear();
    for (auto&[bid, bond]:bondsMap) {
        auto from = bond->getAtomFrom(), to = bond->getAtomTo();
        neighborAtomsMap[from].insert(to);
        neighborAtomsMap[to].insert(from);
        neighborBondsMap[from].insert(bid);
        neighborBondsMap[to].insert(bid);
    }
}

std::pair<std::vector<std::shared_ptr<JAtom>>, std::vector<std::shared_ptr<JBond>>>
JMol::makeCarbonRing(const size_t &_nodeNum,
                     const vector<std::pair<size_t, size_t>> &_singleLink,
                     const vector<std::pair<size_t, size_t>> &_doubleLink,
                     const std::vector<size_t> &_mixAtomicNumbers) {

    std::vector<std::shared_ptr<JAtom>> newAtoms;
    for (size_t i = 0; i < _nodeNum; i++) {
        newAtoms.push_back(addAtom(6));
    }
    std::vector<std::shared_ptr<JBond>> newBonds;
    for (auto&[a1, a2]:_singleLink) {
        newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                   newAtoms[a2 - 1]->getId()));
    }
    for (auto&[a1, a2]:_doubleLink) {
        newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                   newAtoms[a2 - 1]->getId(),
                                   JBondType::DoubleBond));
    }
    if (!_mixAtomicNumbers.empty()) {
        for (auto &atom:newAtoms) {
            // TODO: 1：选杂原子 2：检查是否可以插入
        }
    }
    return {std::move(newAtoms), std::move(newBonds)};
}

const vector<std::vector<std::unordered_set<size_t>>> &
JMol::getAromaticRings(bool _retAid) const {
    return _retAid ? aromaticRingAids : aromaticRingBids;
}

void JMol::setAlkane(const string &_alkaneSMILES) {
    clear();
    AlkaneGraph<node_type> alkane;
    alkane.fromSMILES(_alkaneSMILES);
    std::unordered_map<node_type, size_t> a2jAidMap;
    std::unordered_set<std::pair<size_t, size_t>, pair_hash> bMap;
    // FIXME: 消除 DFS 过程的冗余操作
    auto before = [&](const node_type &current, const node_type &from) {
        // 添加节点
        if (notExist(a2jAidMap, current)) {
            a2jAidMap[current] = addAtom(6)->getId();
        }
        if (from != alkane.EMPTY_NODE && notExist(a2jAidMap, from)) {
            a2jAidMap[from] = addAtom(6)->getId();
        }
        // 添加边
        if (from != alkane.EMPTY_NODE) {
            size_t jFrom = a2jAidMap[from], jTo = a2jAidMap[current];
            if (notExist(bMap, std::make_pair(jFrom, jTo))) {
                addBond(jFrom, jTo);
                bMap.insert({jFrom, jTo});
                bMap.insert({jTo, jFrom});
            }
        }
    };
    alkane.dfsWrapper(before, before, before);
}

std::unordered_map<size_t, cv::Point3f> JMol::get3DCoordinates(bool _addHs) {
    // TODO: 要求更新3D坐标的时候必须加氢
    if (_addHs) {
        if (atomValenceMap.empty()) {
            updateAtomValenceMap();
        }
        auto add_all_hydrogen = [&](const size_t &_aid) {
            addHs(_aid);
        };
        safeTraverseAtoms(add_all_hydrogen);
    }
    std::unordered_map<size_t, cv::Point3f> depAtomPosMap;
#ifdef USE_OPENBABEL
    OpenBabel::OBMol obMol;
    obMol.BeginModify();
    for(auto[id,atom]:atomsMap){
        auto obAtom = obMol.NewAtom(id);
        int atomicNumber = atom->getAtomicNumber();
        // bug occurs if add H here. add F instead。 REM to fix it later.
        if (atomicNumber == 1)atomicNumber = 9;
        obAtom->SetAtomicNum(atomicNumber);
    }
    for(auto[id,bond]:bondsMap){
        auto obBond = obMol.NewBond();
        obBond->SetBondOrder(std::lround(bond->asValence().floatValue()));
        obBond->SetBegin(obMol.GetAtomById(bond->getAtomFrom()));
        obBond->SetEnd(obMol.GetAtomById(bond->getAtomTo()));
    }
    obMol.EndModify();
    OpenBabel::OBBuilder builder;
    builder.Build(obMol);
    obMol.AddHydrogens(false, true);
    OpenBabel::OBForceField *pFF = OpenBabel::OBForceField::FindForceField("MMFF94s");
    if (!pFF) {
        std::cerr << "fail to find uff forcefield" << std::endl;
        exit(-1);
    }
    obMol.AddHydrogens(false, true);
    // this func return f
    pFF->Setup(obMol);
//    if (!pFF->Setup(obMol)) {
//        std::cerr << "fail to setup uff forcefield for mol" << std::endl;
//        exit(-1);
//    }
    pFF->SteepestDescent(250, 1.0e-4);
    pFF->WeightedRotorSearch(50, 25);
    pFF->SteepestDescent(500, 1.0e-6);
    pFF->UpdateCoordinates(obMol);
    FOR_ATOMS_OF_MOL(obAtom, obMol) {
        depAtomPosMap[obAtom->GetId()] = cv::Point3f(obAtom->x(), obAtom->y(), obAtom->z());
    }
#elif defined(USE_RDKIT)
    auto[rwMol, j2rAidMap] = convertJMolToRWMol(*this);
    std::unordered_map<unsigned int, size_t> r2jAidMap;
    for (auto&[jAid, rAid]:j2rAidMap) {
        r2jAidMap[rAid] = jAid;
    }
    j2rAidMap.clear();
    for (unsigned int i = 0; i < rwMol->getNumAtoms(); i++) {
        rwMol->getAtomWithIdx(i)->calcImplicitValence(true);
        rwMol->getAtomWithIdx(i)->calcExplicitValence(true);
    }
//    RDKit::MolOps::findSSSR(*rwMol, nullptr);
    RDKit::MolOps::fastFindRings(*rwMol);
    int confId = RDKit::DGeomHelpers::EmbedMolecule(*rwMol, 1000, 1234);
//    auto[state, energy]=RDKit::UFF::UFFOptimizeMolecule(
//            *rwMol, 1000, 10.0, confId,true);
//    std::cout << "state=" << state << ",energy=" << energy << std::endl;
    RDKit::MMFF::MMFFOptimizeMolecule(*rwMol, 10000, "MMFF94", 10.0, confId);
    auto conf = rwMol->getConformer(confId);
    for (unsigned int i = 0; i < rwMol->getNumAtoms(); i++) {
        auto pos = conf.getAtomPos(i);
        depAtomPosMap[r2jAidMap[i]] = cv::Point3f(pos.x, pos.y, pos.z);
    }
#endif //! USE_OPENBABEL
    return std::move(depAtomPosMap);
}

void JMol::safeTraverseAtoms(const function<void(const size_t &)> &func) {
    std::vector<size_t> aids;
    for (auto&[id, _]:atomsMap) {
        aids.push_back(id);
    }
    for (auto &aid:aids) {
        func(aid);
    }
}

const unordered_map<size_t, std::pair<bool, cv::Point2f>> &JMol::getAtomPosMap2D() const {
    return atomPosMap2D;
}

void JMol::insertAtomPos2D(const size_t &_aid, bool _isExplicit, const cv::Point2f &_pos) {
    atomPosMap2D[_aid] = {_isExplicit, _pos};
}

std::vector<std::shared_ptr<JMol>> JMol::split() const {
    AlkaneGraph<size_t> graph;
    std::unordered_map<size_t, size_t> j2gAidMap, g2jMap;
    std::vector<std::shared_ptr<JMol>> result;
    size_t gAidIdx = 0;
    for (auto&[aid, atom]:atomsMap) {
        j2gAidMap[aid] = gAidIdx++;
    }
//    for(auto&[jAid,gAid]:j2gAidMap){
//        graph.push_node(gAid);
//    }
//    graph.push_node(gAidIdx - 1);
    for (auto&[bid, bond]:bondsMap) {
        graph.push_back(j2gAidMap[bond->getAtomFrom()], j2gAidMap[bond->getAtomTo()]);
    }
    for (auto&[jAid, gAid]:j2gAidMap) {
        g2jMap[gAid] = jAid;
    }
    // 保证一个连通片不和其它连通片共享边 or 节点
    std::vector<std::unordered_set<size_t>> groups = graph.dfsTraceGroup();
    for (auto &group:groups) {
        auto mol = std::make_shared<JMol>();
        std::unordered_map<size_t, size_t> aidMap;//<old,new>
        for (auto &gid:group) {
            size_t oldAid = g2jMap[gid];
            auto &atom = atomsMap.find(oldAid)->second;
            auto newAtom = mol->addAtom(atom->getAtomicNumber());
            aidMap[oldAid] = newAtom->getId();
        }
        for (auto&[bid, bond]:bondsMap) {
            if (notExist(group, bond->getAtomFrom()))continue;
            mol->addBond(aidMap[bond->getAtomFrom()], aidMap[bond->getAtomTo()],
                         bond->getBondType());
        }
        result.push_back(std::move(mol));
    }
    return result;
}

