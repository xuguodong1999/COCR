#include "mol.hpp"

#include <GraphMol/GraphMol.h>
#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/MolOps.h>

JMol::JMol(const string &_smiles) {
    set(_smiles);
}

void JMol::set(const string &_smiles) {
    clear();
    std::map<unsigned int, size_t> R2JAtomIdxMap;
    std::shared_ptr<RDKit::ROMol> roMol(RDKit::SmilesToMol(_smiles));
    for (unsigned int i = 0; i < roMol->getNumAtoms(); i++) {
        const RDKit::Atom *rAtom = roMol->getAtomWithIdx(i);
        auto jAtom = addAtom(rAtom->getAtomicNum());
        R2JAtomIdxMap.insert(std::make_pair(rAtom->getIdx(), jAtom->getId()));

    }
    for (unsigned int i = 0; i < roMol->getNumBonds(false); i++) {
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
}
