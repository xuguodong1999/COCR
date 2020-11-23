#include "mol.hpp"
#include "std_util.hpp"

#include <GraphMol/Chirality.h>
#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/Depictor/RDDepictor.h>
#include <GraphMol/AtomIterators.h>
#include <random>

using namespace std;

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
    atomPosMap.clear();
    mAids = mBids = std::numeric_limits<int>::lowest();
}

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
convertJBondTyteToRDKitBondType(const JBondType &_bondType) {
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
        auto[stereo, dir]=convertJBondTyteToRDKitBondType(jBond->getBondType());
        rwMol->addBond(j2rAidMap[jBond->getAtomFrom()],
                       j2rAidMap[jBond->getAtomTo()],
                       stereo);
        rwMol->getBondBetweenAtoms(j2rAidMap[jBond->getAtomFrom()],
                                   j2rAidMap[jBond->getAtomTo()])->setBondDir(dir);
    }
    return {std::move(rwMol), std::move(j2rAidMap)};
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


void JMol::update2DCoordinates() {
    atomPosMap.clear();
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
}

const unordered_map<size_t, cv::Point2f> &JMol::getAtomPosMap() const {
    return atomPosMap;
}

void JMol::randomize(const float &_addHydrogenProb) {
    // 维护化合价表
    std::unordered_map<size_t, frac> atomValenceMap;
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
    // 换化学键类型
    static std::vector<JBondType> sBondCandidates = {
            JBondType::SingleBond, JBondType::DoubleBond, JBondType::TripleBond};
    for (auto&[id, bond]:bondsMap) {
        const size_t &from = bond->getAtomFrom(), to = bond->getAtomTo();
        const auto &fromAtom = atomsMap[from];
        const auto &toAtom = atomsMap[to];
        const frac deltaFrom = ElementValenceData[fromAtom->getElementType()]
                               - atomValenceMap[from];
        const frac deltaTo = ElementValenceData[toAtom->getElementType()]
                             - atomValenceMap[to];
        if (deltaFrom >= 1 && deltaTo >= 1) {
            frac delta = std::min(deltaFrom, deltaTo);
            size_t candidatesSize = sBondCandidates.size();
            if (delta >= 2) {// 三键、双键
                candidatesSize = candidatesSize;
            } else {// 双键
                candidatesSize -= 1;
            }
            // 检查键端原子是不是已经有双键
            bool allowReplace = true;
            for (auto&[id2, bond2]:bondsMap) {
                if (id == id2) {
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
                bond->setType(sBondCandidates[rand() % candidatesSize]);
                atomValenceMap[from] += (bond->asValence() - oldValence);
                atomValenceMap[to] += (bond->asValence() - oldValence);
            }
        }
    }
    // 换原子类型
    static std::vector<ElementType> sAtomCandidates = {
            ElementType::H, ElementType::C,
            ElementType::N, ElementType::P, ElementType::B,
            ElementType::O, ElementType::S,
            ElementType::F, ElementType::Cl, ElementType::Br, ElementType::I
    };
    for (auto&[id, atom]:atomsMap) {
        const auto &ele = sAtomCandidates[rand() % sAtomCandidates.size()];
        if (atomValenceMap[id] <= ElementValenceData[ele]) {
            atom->setElementType(ele);
        }
    }
    // 添加杂环、官能团
    for (auto&[id, atom]:atomsMap) {// COOH CHO

    }
    std::vector<size_t> replacedBonds;
    for (auto&[id, bond]:bondsMap) {// c1ccccc1 c1cncc1
        if (byProb(0.5) && JBondType::SingleBond == bond->getBondType()) {
            replacedBonds.push_back(id);
        }
    }
    for (auto &bid:replacedBonds) {
        addAromaticRing(bid, atomValenceMap);
    }
    // 打补丁：删掉 P#C B#C
    for (auto&[id, bond]:bondsMap) {// c1ccccc1 c1cncc1
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
    }
    // 添加氢原子
    for (auto&[id, atom]:atomsMap) {
        if (ElementType::C != atom->getElementType()) {// 杂原子较大可能加氢
            if (byProb(1.0 - _addHydrogenProb)) {
                addHs(id, atomValenceMap);
            }
        } else if (byProb(_addHydrogenProb)) {// 碳原子按概率加氢
            addHs(id, atomValenceMap);
        }
    }
    // TODO: 添加表达方式（光学异构、凯库勒式）：楔形键、波浪线、环等
    // TODO: 添加不展开的字符串
}

void JMol::addHs(const size_t &_aid, std::unordered_map<size_t, frac> &_atomValenceMap) {
    auto &valence = _atomValenceMap.at(_aid);
    const auto &atom = atomsMap[_aid];
    frac numOfH = ElementValenceData[atom->getElementType()] - valence;
    if (numOfH >= 1) {
        int hsNum = std::round(numOfH.floatValue());
        for (int i = 0; i < hsNum; i++) {
            auto h = addAtom(1);
            const size_t &hid = h->getId();
            addBond(hid, _aid, JBondType::SingleBond);
            _atomValenceMap[hid] = 1;
        }
        valence += numOfH;
    }
}

std::string JMol::toSMILES(bool _addRandomStereo) const {
    auto[rwMol, _] = convertJMolToRWMol(*this);
    if (_addRandomStereo) {
        static std::vector<RDKit::Atom::ChiralType> cand = {
                RDKit::Atom::ChiralType::CHI_TETRAHEDRAL_CW,
                RDKit::Atom::ChiralType::CHI_TETRAHEDRAL_CCW
        };
        for (unsigned int i = 0; i < rwMol->getNumAtoms(); i++) {
            rwMol->getAtomWithIdx(i)->calcImplicitValence(true);
        }
        auto stereos = RDKit::Chirality::findPotentialStereo((*rwMol.get()), false);
        for (auto &stereo:stereos) {
//            std::cout << static_cast<int>(stereo.type) << std::endl;
            if(stereo.centeredOn!=RDKit::Chirality::StereoInfo::NOATOM) {
                rwMol->getAtomWithIdx(stereo.centeredOn)->
                        setChiralTag(cand[rand() % cand.size()]);
            }
        }
//        RDKit::
//        for (unsigned int i = 0; i < rwMol->getNumAtoms(); i++) {
//            if (rwMol->getAtomWithIdx(i))
//                rwMol->getAtomWithIdx(i)->setChiralTag(cand[rand() % cand.size()]);
//        }
    }
    auto roMol = std::make_shared<RDKit::ROMol>(*(rwMol.get()));
    return RDKit::MolToSmiles(
            *(roMol.get()), true, false, -1,
            true, false, false, false);
}

void JMol::addAromaticRing(const size_t &_bid,
                           std::unordered_map<size_t, frac> &_atomValenceMap) {
    std::vector<size_t> atomicNumbers;
    std::vector<std::shared_ptr<JAtom>> newAtoms;
    std::vector<std::shared_ptr<JBond>> newBonds;
    auto from = bondsMap[_bid]->getAtomFrom(), to = bondsMap[_bid]->getAtomTo();
    removeBond(_bid, _atomValenceMap);
    int index = rand() % 6;
    switch (index) {
        case 0:
        case 1:
        case 2:
        case 3: {// 呋喃系
            atomicNumbers = {7, 8, 15, 16};
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
            atomicNumbers = {6, 6, 6, 7};
            newAtoms = {
                    addAtom(6),
                    addAtom(6),
                    addAtom(atomicNumbers[rand() % atomicNumbers.size()]),
                    addAtom(atomicNumbers[rand() % atomicNumbers.size()]),
                    addAtom(atomicNumbers[rand() % atomicNumbers.size()]),
                    addAtom(atomicNumbers[rand() % atomicNumbers.size()])
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
    auto addValence4Atom = [&](const size_t &_aid, const frac &_valence) {
        if (_atomValenceMap.end() == _atomValenceMap.find(_aid)) {
            _atomValenceMap[_aid] = _valence;
        } else {
            _atomValenceMap[_aid] += _valence;
        }
    };
    for (auto &bond:newBonds) {
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
    }
}

void JMol::addCommonRing(const size_t &_bid,
                         std::unordered_map<size_t, frac> &_atomValenceMap) {

}

void JMol::removeBond(const size_t &_bid, unordered_map<size_t, frac> &_atomValenceMap) {
    auto it = bondsMap.find(_bid);
    if (bondsMap.end() != it) {
        auto oldVal = it->second->asValence();
        _atomValenceMap[it->second->getAtomFrom()] -= oldVal;
        _atomValenceMap[it->second->getAtomTo()] -= oldVal;
        bondsMap.erase(it);
    }
}






