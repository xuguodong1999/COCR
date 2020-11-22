#include "mol.hpp"

#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/Depictor/RDDepictor.h>
#include <GraphMol/AtomIterators.h>

using namespace std;

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

std::pair<std::shared_ptr<RDKit::RWMol>,std::unordered_map<size_t, unsigned int>>
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
    return {std::move(rwMol),std::move(j2rAidMap)};
}


void JMol::set(const string &_smiles) {
    clear();
    // the caller is responsible for freeing this.
    auto rwMol = RDKit::SmilesToMol(_smiles, 0, false);
    std::map<unsigned int, size_t> r2jAidMap;
    for(auto it=rwMol->beginAtoms();it!=rwMol->endAtoms();it++){
        const auto rAtom = (*it);
        auto jAtom = addAtom(rAtom->getAtomicNum());
        r2jAidMap.insert({rAtom->getIdx(), jAtom->getId()});
    }
    // FIXME: H can't be explicitly added here
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
    auto[ rwMol,j2rAidMap] = convertJMolToRWMol(*this);
    std::unordered_map<unsigned int,size_t> r2jAidMap;
    for(auto&[jAid,rAid]:j2rAidMap){
        r2jAidMap[rAid]=jAid;
    }
    j2rAidMap.clear();
    auto roMol = std::make_shared<RDKit::ROMol>(*(rwMol.get()));
    RDDepict::compute2DCoords(*(roMol.get()));
    auto conf = roMol->getConformer();
    for(auto it=roMol->beginAtoms();it!=roMol->endAtoms();it++){
        auto idx=(*it)->getIdx();
        auto pos = conf.getAtomPos(idx);
        atomPosMap[r2jAidMap[idx]]=cv::Point2f(pos.x,pos.y);
    }
}


void JMol::randomize() {
    // 换原子类型
    // 换化学键类型
    // 添加杂环、官能团
    // 添加氢原子、杂原子
    // 添加不展开的字符串
}

std::shared_ptr<JAtom> JMol::removeAtom(const size_t &_aid) {
    return std::shared_ptr<JAtom>();
}

std::shared_ptr<JBond> JMol::removeBond(const size_t &_bid) {
    return std::shared_ptr<JBond>();
}

void JMol::addHs(const size_t &_aid) {
    frac valence = 0;
    for (auto&[id, bond]:bondsMap) {
        if (bond->getAtomFrom() == _aid || bond->getAtomTo() == _aid) {
            valence += bond->asValence();
        }
    }
    const auto &atom = atomsMap[_aid];
    frac numOfH = ElementValenceData[atom->getElementType()] - valence;
    if (numOfH >= 1) {
        int hsNum = std::round(numOfH.floatValue());
        for (int i = 0; i < hsNum; i++) {
            auto h = addAtom(1);
            addBond(h->getId(), _aid, JBondType::SingleBond);
        }
    }
}

bool JMol::addRing(const string &_ringName, const size_t &_bid,
                   const int &_pos1, const int &_pos2) {
    return false;
}

const unordered_map<size_t, cv::Point2f> &JMol::getAtomPosMap() const {
    return atomPosMap;
}





