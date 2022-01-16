#include "mol_op.hpp"
#include "std_util.hpp"
#include <random>

void MolOp::clear() {
    MolHolder::clear();
    atomValenceMap.clear();
    neighborAtomsMap.clear();
    neighborBondsMap.clear();
}

void MolOp::addHs(const size_t &_aid) {
    if (atomValenceMap.empty()) {
        updateAtomValenceMap();
    }
    auto &valence = atomValenceMap.at(_aid);
    auto atom = mol->getAtomById(_aid);
    frac numOfH = ElementValenceData[atom->getElementType()] - valence;
    if (numOfH >= 1) {
        int hsNum = std::lround(numOfH.floatValue());
        for (int i = 0; i < hsNum; i++) {
            auto h = mol->addAtom(1);
            const size_t &hid = h->getId();
            mol->addBond(hid, _aid, JBondType::SingleBond);
            atomValenceMap[hid] = 1;
        }
        valence += numOfH;
    }
}

void MolOp::updateNeighborInfoMap() {
    neighborAtomsMap.clear();
    neighborBondsMap.clear();
    auto get_neighbor = [&](const size_t &_bid) {
        auto bond = mol->getBondById(_bid);
        auto from = bond->getAtomFrom(), to = bond->getAtomTo();
        neighborAtomsMap[from].insert(to);
        neighborAtomsMap[to].insert(from);
        neighborBondsMap[from].insert(_bid);
        neighborBondsMap[to].insert(_bid);
    };
    mol->safeTraverseBonds(get_neighbor);
}

void MolOp::updateAtomValenceMap() {
    atomValenceMap.clear();
    auto add_valence4atom = [&](const size_t &_aid, const frac &_valence) {
        if (notExist(atomValenceMap, _aid)) {
            atomValenceMap[_aid] = _valence;
        } else {
            atomValenceMap[_aid] += _valence;
        }
    };
    auto add_valence_from_bond = [&](const size_t &_bid) {
        auto bond = mol->getBondById(_bid);
        add_valence4atom(bond->getAtomFrom(), bond->asValence());
        add_valence4atom(bond->getAtomTo(), bond->asValence());
    };
    mol->safeTraverseBonds(add_valence_from_bond);
    auto handle_atom_alone = [&](const size_t &_aid) {
        if (notExist(atomValenceMap, _aid)) {
            atomValenceMap[_aid] = 0;
        }
    };
    mol->safeTraverseAtoms(handle_atom_alone);
}

void MolOp::addValence4Atom(const size_t &_aid, const frac &_valence) {
    if (notExist(atomValenceMap, _aid)) {
        atomValenceMap[_aid] = _valence;
    } else {
        atomValenceMap[_aid] += _valence;
    }
}

void MolOp::removeBond(const size_t &_bid) {
    auto bond = mol->getBondById(_bid);
    if (bond) {
        auto oldVal = bond->asValence();
        atomValenceMap[bond->getAtomFrom()] -= oldVal;
        atomValenceMap[bond->getAtomTo()] -= oldVal;
        mol->removeBond(_bid);
    }
}

void MolOp::updateAtomValenceMap(const std::vector<std::shared_ptr<JBond>> &_bonds) {
    for (auto &bond:_bonds) {
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
    }
}

MolOp::MolOp(std::shared_ptr<JMol> _mol) : MolHolder(std::move(_mol)) {
}

bool MolOp::markAsDouble(const size_t &_bid) {
    if (atomValenceMap.empty()) {
        updateAtomValenceMap();
    }
    if (dbCounter.empty()) {
        updateDoubleBondCounter();
    }
    auto bond = mol->getBondById(_bid);
    auto a1 = mol->getAtomById(bond->getAtomFrom());
    auto a2 = mol->getAtomById(bond->getAtomTo());
    auto &va1 = atomValenceMap[a1->getId()];
    auto &va2 = atomValenceMap[a2->getId()];
    const auto &oa1 = ElementValenceData[a1->getElementType()];
    const auto &oa2 = ElementValenceData[a2->getElementType()];
    auto d1 = oa1 - va1, d2 = oa2 - va2;
    if (d1 >= 1 && d2 >= 1 &&
        0 == dbCounter[a1->getId()] && 0 == dbCounter[a2->getId()]) {
        dbCounter[a1->getId()] += 1;
        dbCounter[a2->getId()] += 1;
        va1 += (frac(2) - bond->asValence());
        va2 += (frac(2) - bond->asValence());
        bond->setType(JBondType::DoubleBond);
        return true;
    }
    return false;
}

void MolOp::updateDoubleBondCounter() {
    dbCounter.clear();
    auto add_db_num = [&](const size_t &_bid) -> void {
        auto bond = mol->getBondById(_bid);
        bool isDb = JBondType::DoubleBond == bond->getBondType();
        auto a1 = bond->getAtomFrom(), a2 = bond->getAtomTo();
        if (notExist(dbCounter, a1)) {
            dbCounter[a1] = 0;
        }
        if (notExist(dbCounter, a2)) {
            dbCounter[a2] = 0;
        }
        if (isDb) {
            dbCounter[a1] += 1;
            dbCounter[a2] += 1;
        }
    };
    mol->safeTraverseBonds(add_db_num);
}
