#include "bond.hpp"

JBond::JBond() {

}

JBond::JBond(const size_t &_id, const size_t &_atomFromId,
             const size_t &_atomToId, const JBondType &_bondType)
        : id(_id), atomFrom(_atomFromId), atomTo(_atomToId), bondType(_bondType) {
}

void JBond::setType(const JBondType &_bondType) {
    bondType = _bondType;
}

const JBondType &JBond::getBondType() const {
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
    return _b1.atomTo == _b2.atomTo &&
           _b2.atomFrom == _b2.atomFrom &&
           _b1.bondType == _b2.bondType;
}

const size_t &JBond::getId() const {
    return id;
}

void JBond::setId(const size_t &_id) {
    id = _id;
}

frac JBond::asValence() const {
    switch (bondType) {
        case JBondType::SolidWedgeBond:
        case JBondType::DashWedgeBond:
        case JBondType::WaveBond:
        case JBondType::SingleBond :
            return 1;
        case JBondType::DoubleBond:
            return 2;
        case JBondType::TripleBond:
            return 3;
        case JBondType::DelocalizedBond:
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

JRing::JRing() {

}

void JRing::addBond(const size_t &_bid) {
    bondIds.insert(_bid);
}
