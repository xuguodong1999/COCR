#include "XGD/Chem/Bond.h"
#include "XGD/Chem/Atom.h"
#include "XGD/SpdlogStream/Stream.h"

Bond::Bond() {

}

Bond::Bond(const id_type &_id, const id_type &_atomFromId,
           const id_type &_atomToId, const BondType &_bondType)
        : id(_id), atomFrom(_atomFromId), atomTo(_atomToId), bondType(_bondType) {
}

void Bond::setType(const BondType &_bondType) {
    bondType = _bondType;
}

const BondType &Bond::getBondType() const {
    return bondType;
}

const id_type &Bond::getAtomFrom() const {
    return atomFrom;
}

const id_type &Bond::getAtomTo() const {
    return atomTo;
}

void Bond::setFromTo(const id_type &_atomFrom, const id_type &_atomTo) {
    atomFrom = _atomFrom;
    atomTo = _atomTo;
}

bool operator==(const Bond &_b1, const Bond &_b2) {
    return _b1.getAtomTo() == _b2.getAtomTo() &&
           _b2.getAtomFrom() == _b2.getAtomFrom() &&
           _b1.getBondType() == _b2.getBondType();
}

void Bond::setId(const id_type &_id) {
    id = _id;
}

frac Bond::asValence() const {
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
            xgd::serr << "unhandled valence for " << static_cast<int>(bondType) << std::endl;
            exit(-1);
        }
    }
}

bool Bond::setFrom(const id_type &_atomFrom) {
    if (_atomFrom == atomFrom) {
        return true;
    } else if (_atomFrom == atomTo) {
        std::swap(atomFrom, atomTo);
        return true;
    } else {
        return false;
    }
}


Bond::Bond(const id_type &_id, std::shared_ptr<Atom> _from, std::shared_ptr<Atom> _to, const BondType &_type,
           const float &_offset1, const float &_offset2)
        : id(_id), bondType(_type), from(std::move(_from)), to(std::move(_to)), offset1(_offset1), offset2(_offset2) {
}

std::shared_ptr<Atom> Bond::getFrom() const {
    return from;
}

std::shared_ptr<Atom> Bond::getTo() const {
    return to;
}

id_type Bond::getId() const {
    return id;
}

BondType Bond::getType() const {
    return bondType;
}

int Bond::getBondOrder() const {
    switch (bondType) {
        case BondType::SingleBond:
        case BondType::SolidWedgeBond:
        case BondType::DashWedgeBond:
            return 1;
        case BondType::DoubleBond:
            return 2;
        case BondType::TripleBond:
            return 3;
        default:
            return 1;// in openbabel, BondType::DelocalizedBond is 5
    }
}

void Bond::setOrder(const int &_order) {
    switch (_order) {
        case 2:
            bondType = BondType::DoubleBond;
            break;
        case 3:
            bondType = BondType::TripleBond;
            break;
        default:
            bondType = BondType::SingleBond;
    }
}

float Bond::getFromOffset() const {
    return offset1;
}

float Bond::getToOffset() const {
    return offset2;
}

void Bond::setFrom(const std::shared_ptr<Atom> &from) {
    Bond::from = from;
}

void Bond::setTo(const std::shared_ptr<Atom> &to) {
    Bond::to = to;
}

std::string Bond::getName() const {
    switch (bondType) {
        case BondType::SingleBond:
            return "Single";
        case BondType::DoubleBond:
            return "Double";
        case BondType::TripleBond:
            return "Triple";
        case BondType::DelocalizedBond:
            return "Delocalized";
        case BondType::WaveBond:
            return "Implicit";
        case BondType::SolidWedgeBond:
            return "UpBond";
        case BondType::DashWedgeBond:
            return "Down";
        default:
            return "Unknown";
    }
}
