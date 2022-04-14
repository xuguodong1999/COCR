#include "bond.h"
using namespace v1_0;
JBond::JBond() {

}

JBond::JBond(const size_t &_id, const size_t &_atomFromId,
             const size_t &_atomToId, const BondType &_bondType)
        : id(_id), atomFrom(_atomFromId), atomTo(_atomToId), bondType(_bondType) {
}

void JBond::setType(const BondType &_bondType) {
    bondType = _bondType;
}

const BondType &JBond::getBondType() const {
    return bondType;
}

const size_t &JBond::getAtomFrom() const {
    return atomFrom;
}

const size_t &JBond::getAtomTo() const {
    return atomTo;
}

void JBond::setFromTo(const size_t &_atomFrom, const size_t &_atomTo) {
    atomFrom = _atomFrom;
    atomTo = _atomTo;
}

bool operator==(const JBond &_b1, const JBond &_b2) {
    return _b1.getAtomTo() == _b2.getAtomTo() &&
           _b2.getAtomFrom() == _b2.getAtomFrom() &&
           _b1.getBondType() == _b2.getBondType();
}

const size_t &JBond::getId() const {
    return id;
}

void JBond::setId(const size_t &_id) {
    id = _id;
}

frac JBond::asValence() const {
    switch (bondType) {
        case BondType::SolidWedgeBond:
        case BondType::DashWedgeBond:
        case BondType::WaveBond:
        case BondType::SingleBond :
            return 1;
        case BondType::DoubleBond:
            return 2;
        case BondType::TripleBond:
            return 3;
        case BondType::DelocalizedBond:
            return frac(3, 2);
        default: {
            std::cerr << "unhandled valence for " << static_cast<int>(bondType) << std::endl;
            exit(-1);
        }
    }
}

bool JBond::setFrom(const size_t &_atomFrom) {
    if (_atomFrom == atomFrom) {
        return true;
    } else if (_atomFrom == atomTo) {
        std::swap(atomFrom, atomTo);
        return true;
    } else {
        return false;
    }
}