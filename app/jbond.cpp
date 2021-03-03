#include "jbond.hpp"

xgd::JBond::JBond(const size_t &_id, const BondType &_type, std::shared_ptr<JAtom> _from, std::shared_ptr<JAtom> _to)
        : id(_id), type(_type), from(std::move(_from)), to(std::move(_to)) {
}

std::shared_ptr<xgd::JAtom> xgd::JBond::getFrom() const {
    return from;
}

std::shared_ptr<xgd::JAtom> xgd::JBond::getTo() const {
    return to;
}

size_t xgd::JBond::getId() const {
    return id;
}

xgd::BondType xgd::JBond::getType() const {
    return type;
}

void xgd::JBond::setType(const xgd::BondType &_type) {
    type = _type;
}

int xgd::JBond::getBondOrder() const {
    switch (type) {
        case BondType::SingleBond:
        case BondType::UpBond:
        case BondType::DownBond:
            return 1;
        case BondType::DoubleBond:
            return 2;
        case BondType::TripleBond:
            return 3;
        default:
            return 1;// in openbabel, BondType::DelocalizedBond is 5
    }
}

