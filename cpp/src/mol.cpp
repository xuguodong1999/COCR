#include "mol.hpp"

#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/Depictor/RDDepictor.h>

#ifndef _DEBUG
RDLogger rdErrorLog;
#endif

/**
 * 没有转换光学异构信息
 * @param _jmol
 * @return
 */
std::shared_ptr<RDKit::ROMol> convertJMolToROMol(const JMol &_jmol) {
    std::map<size_t, unsigned int> J2RAtomMap;
    auto rwMol = std::make_shared<RDKit::RWMol>();
    for (auto&[id, atom]:_jmol.getAtomsMap()) {
        auto a = std::make_shared<RDKit::Atom>(static_cast<int>(atom->getElementType()));
        auto rid = rwMol->addAtom(a.get(), true, false);
        J2RAtomMap.insert(std::make_pair(id, rid));
    }

    for (auto&[id, bond]:_jmol.getBondsMap()) {
        rwMol->addBond(J2RAtomMap[bond->getAtomFrom()], J2RAtomMap[bond->getAtomTo()],
                // FIXME: make sure enum types in rdkit and jmol are equal.
                       static_cast<RDKit::Bond::BondType>(bond->getBondType()));

    }
    return std::move(rwMol);
}

JMol::JMol(const string &_smiles) {
    set(_smiles);
}

void JMol::set(const string &_smiles) {
    clear();
    std::map<unsigned int, size_t> R2JAtomIdxMap;
    // FIXME: memory leak
    auto rwMol = RDKit::SmilesToMol(_smiles);
    std::shared_ptr<RDKit::ROMol> roMol(rwMol);
    for (unsigned int i = 0; i < roMol->getNumAtoms(); i++) {
        const RDKit::Atom *rAtom = roMol->getAtomWithIdx(i);
        auto jAtom = addAtom(rAtom->getAtomicNum());
        R2JAtomIdxMap.insert(std::make_pair(rAtom->getIdx(), jAtom->getId()));
    }
    // FIXME: H can't be explicitly added here
    for (unsigned int i = 0; i < roMol->getNumBonds(true); i++) {
        const RDKit::Bond *rBond = roMol->getBondWithIdx(i);
        auto jBond = addBond(R2JAtomIdxMap[rBond->getBeginAtomIdx()],
                             R2JAtomIdxMap[rBond->getEndAtomIdx()]);
        switch (rBond->getBondType()) {
            case RDKit::Bond::BondType::SINGLE: {
                switch (rBond->getBondDir()) {
                    case RDKit::Bond::BondDir::BEGINWEDGE:
                        jBond->setType(JBondType::SolidWedgeBond);
                        break;
                    case RDKit::Bond::BondDir::BEGINDASH:
                        jBond->setType(JBondType::DashWedgeBond);
                        break;
                    case RDKit::Bond::BondDir::NONE:
                        jBond->setType(JBondType::SingleBond);
                        break;
                    default:
                        std::cerr << "unhandled direction:" << rBond->getBondDir() << std::endl;
                        exit(-1);
                }
                break;
            }
            case RDKit::Bond::BondType::DOUBLE:
                jBond->setType(JBondType::DoubleBond);
                break;
            case RDKit::Bond::BondType::TRIPLE:
                jBond->setType(JBondType::TripleBond);
                break;
            default:
                // FIXME: fx结构强制转kekule结构
                std::cerr << "unhandled type:" << rBond->getBondType() << std::endl;
                exit(-1);
        }
    }
}

std::shared_ptr<JAtom> JMol::addAtom(const size_t &_atomicNumber) {
    auto atom = std::make_shared<JAtom>(_atomicNumber);
    atomsMap.insert(std::make_pair(atom->getId(), atom));
    return std::move(atom);
}

std::shared_ptr<JBond> JMol::addBond(const size_t &_atomFromId, const size_t &_atomToId) {
    auto bond = std::make_shared<JBond>(_atomFromId, _atomToId);
    bondsMap.insert(std::make_pair(bond->getId(), bond));
    return std::move(bond);
}

std::shared_ptr<JBond> JMol::addBond(
        const size_t &_atomFromId, const size_t &_atomToId, const JBondType &_bondType) {
    auto bond = addBond(_atomFromId, _atomToId);
    bond->setType(_bondType);
    return std::move(bond);
}

void JMol::clear() {
    bondsMap.clear();
    atomsMap.clear();
    atomsPosMap.clear();
}

void JMol::update2DCoordinates() {
    atomsPosMap.clear();
    auto rwMol = convertJMolToROMol(*this);
    auto roMol = std::make_shared<RDKit::ROMol>(*(rwMol.get()));
    RDDepict::compute2DCoords(*(roMol.get()));
    auto conf = roMol->getConformer();
    std::cout << "scatter([";
    for (unsigned int i = 0; i < roMol->getNumAtoms(); i++) {
        const RDKit::Atom *rAtom = roMol->getAtomWithIdx(i);
        auto pos = conf.getAtomPos(rAtom->getIdx());
        std::cout << pos.x << ",";
    }
    std::cout << "],[";
    for (unsigned int i = 0; i < roMol->getNumAtoms(); i++) {
        const RDKit::Atom *rAtom = roMol->getAtomWithIdx(i);
        auto pos = conf.getAtomPos(rAtom->getIdx());
        std::cout << pos.y << ",";
    }
    std::cout << "],'K*');\naxis([-5,5,-5,5]);grid on;" << std::endl;
    std::cin.get();
}

const std::map<size_t, std::shared_ptr<JBond>> &JMol::getBondsMap() const {
    return bondsMap;
}

const std::map<size_t, std::shared_ptr<JAtom>> &JMol::getAtomsMap() const {
    return atomsMap;
}
