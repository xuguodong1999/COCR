#include "mol.hpp"
#include "std_util.hpp"

#define USE_COORDGEN2D
#ifdef USE_COORDGEN2D

#include <coordgen/sketcherMinimizer.h>

#else
#include <GraphMol/Depictor/RDDepictor.h>
#endif

#include <GraphMol/Chirality.h>
#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/AtomIterators.h>
#include <random>

using namespace std;

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
    mAids = mBids = std::numeric_limits<int>::lowest();
}

void JMol::clear() {
    bondsMap.clear();
    atomsMap.clear();
    atomValenceMap.clear();
    neighborAtomsMap.clear();
    neighborBondsMap.clear();
    mAids = mBids = std::numeric_limits<int>::lowest();
}

void JMol::set(const string &_smiles) {
    clear();
    // the caller is responsible for freeing this.
    auto rwMol = RDKit::SmilesToMol(_smiles, 0, false);
    std::map<unsigned int, size_t> r2jAidMap;
    for (auto it = rwMol->beginAtoms(); it != rwMol->endAtoms(); it++) {
        const auto rAtom = (*it);
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
}

float JMol::getFontSize() const {
#ifdef USE_COORDGEN2D
    return 30;
#else
    return 1;
#endif
}

std::unordered_map<size_t, cv::Point2f> JMol::get2DCoordinates() const {
    std::unordered_map<size_t, cv::Point2f> atomPosMap;
#ifdef USE_COORDGEN2D
    sketcherMinimizer minimizer;
    sketcherMinimizerMolecule sMol; // This lib is written by Schrodinger. Inc.
    std::unordered_map<size_t, sketcherMinimizerAtom *> j2cAtomMap;
    for (auto&[id, atom]:atomsMap) {
        auto a = sMol.addNewAtom();
        a->setAtomicNumber(atom->getAtomicNumber());
        j2cAtomMap[id] = a;
    }
    std::vector<sketcherMinimizerBond *> cBondVec;
    for (auto&[id, bond]:bondsMap) {
        auto b = sMol.addNewBond(j2cAtomMap[bond->getAtomFrom()],
                                 j2cAtomMap[bond->getAtomTo()]);
        b->setBondOrder(std::lround(bond->asValence().floatValue()));
        cBondVec.push_back(b);
    }
    minimizer.initialize(&sMol);
    minimizer.runGenerateCoordinates();
    for (auto&[aid, cAtom]:j2cAtomMap) {
        auto pos = cAtom->getCoordinates();
        atomPosMap[aid] = cv::Point2f(pos.x(), pos.y());
    }
    minimizer.clear();
    for (auto&[_, i]:j2cAtomMap) {
        delete i;
        i = nullptr;
    }
    for (auto &i:cBondVec) {
        delete i;
        i = nullptr;
    }
#else
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
    for (auto it = roMol->beginAtoms(); it != roMol->endAtoms(); it++) {
        auto idx = (*it)->getIdx();
        auto pos = conf.getAtomPos(idx);
        atomPosMap[r2jAidMap[idx]] = cv::Point2f(pos.x, pos.y);
    }
#endif
    return std::move(atomPosMap);
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
    for (auto&[id, atom]:atomsMap) {// COOH CHO etc
        if (atomValenceMap[id] == 1 && ElementType::C == atom->getElementType()) {
            addGroup(id);
        }
    }
    // 添加杂环
    std::unordered_set<size_t> usedAids;
    auto replaceBondWithRing = [&](const size_t &_bid) {
        const auto &bond = bondsMap[_bid];
        if (byProb(0.5) && JBondType::SingleBond == bond->getBondType()) {
            size_t from = bondsMap[_bid]->getAtomFrom(), to = bondsMap[_bid]->getAtomTo();
            // 避免一个原子挂上好几个环导致排版引擎崩溃
            if (usedAids.end() == usedAids.find(from) && usedAids.end() == usedAids.find(to)) {
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
    for (auto&[id, atom]:atomsMap) {
        if (ElementType::C != atom->getElementType()) {// 杂原子较大可能加氢
            if (byProb(_addHydrogenProb + 0.5)) {
                addHs(id);
            }
        } else if (byProb(_addHydrogenProb)) {// 碳原子按概率加氢
            addHs(id);
        }
    }
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
    // TODO: 波浪线、环
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
    auto[rwMol, _] = convertJMolToRWMol(*this);
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
    auto roMol = std::make_shared<RDKit::ROMol>(*(rwMol.get()));
    return RDKit::MolToSmiles(
            *(roMol.get()), true, false, -1,
            true, false, false, false);
}

void JMol::updateAtomValenceMap() {
    atomValenceMap.clear();
    auto addValence4Atom = [&](const size_t &_aid, const frac &_valence) {
        if (atomValenceMap.end() == atomValenceMap.find(_aid)) {
            atomValenceMap[_aid] = _valence;
        } else {
            atomValenceMap[_aid] += _valence;
        }
    };
    for (auto&[id, bond]:bondsMap) {
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
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
    if (atomValenceMap.end() == atomValenceMap.find(_aid)) {
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
    int index = rand() % 6;
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
                            JBondType::DoubleBond),
                    addBond(from, newAtoms[1 + rand() % 2]->getId(),
                            JBondType::SingleBond),
                    addBond(to, newAtoms[3 + rand() % 2]->getId(),
                            JBondType::SingleBond)
            };
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
                            JBondType::SingleBond),
                    addBond(from, pos1, JBondType::SingleBond),
                    addBond(to, pos2, JBondType::SingleBond)
            };
            break;
        }
        case 6: {
            break;
        }
        case 7: {
            break;
        }
        case 8: {
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
    int index = rand() % 5;
    std::vector<size_t> atomicNumbers = {5, 6, 7, 8, 15, 16};
    switch (index) {
        case 0: {// 三元环
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
        case 1: {// 四元环
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
        case 2: {// 五元环
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
        case 3: {// 六元环
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
        case 4: {// 七元环
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
        case 5: {
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
