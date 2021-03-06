#include "jbond.hpp"
#include "jatom.hpp"
#include <stdexcept>

xgd::JBond::JBond(const size_t &_id, std::shared_ptr<JAtom> _from, std::shared_ptr<JAtom> _to, const BondType &_type)
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

void xgd::JBond::setOrder(const int &_order) {
    switch (_order) {
        case 2:
            type = BondType::DoubleBond;
            break;
        case 3:
            type = BondType::TripleBond;
            break;
        default:
            type = BondType::SingleBond;
    }
}

size_t xgd::JBond::fromId() const {
    if (from)return from->getId();
    throw std::runtime_error("bond with empty from atom");
}

size_t xgd::JBond::toId() const {
    if (to)return to->getId();
    throw std::runtime_error("bond with empty to atom");
}

