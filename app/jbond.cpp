#include "jbond.hpp"
#include "jatom.hpp"
#include "color_types.hpp"
#include <stdexcept>

using namespace xgd;

JBond::JBond(const size_t &_id, std::shared_ptr<JAtom> _from, std::shared_ptr<JAtom> _to, const BondType &_type)
        : id(_id), type(_type), from(std::move(_from)), to(std::move(_to)) {
}

std::shared_ptr<JAtom> JBond::getFrom() const {
    return from;
}

std::shared_ptr<JAtom> JBond::getTo() const {
    return to;
}

size_t JBond::getId() const {
    return id;
}

BondType JBond::getType() const {
    return type;
}

void JBond::setType(const BondType &_type) {
    type = _type;
}

int JBond::getBondOrder() const {
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

void JBond::setOrder(const int &_order) {
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

size_t JBond::fromId() const {
    if (from)return from->getId();
    throw std::runtime_error("bond with empty from atom");
}

size_t JBond::toId() const {
    if (to)return to->getId();
    throw std::runtime_error("bond with empty to atom");
}

static std::unordered_map<BondType, ColorName> colorMap = {
        {BondType::SingleBond,      ColorName::rgbDarkBlue},
        {BondType::DoubleBond,      ColorName::rgbDarkCyan},
        {BondType::TripleBond,      ColorName::rgbLightSkyBlue},
        {BondType::DelocalizedBond, ColorName::rgbYellow},
        {BondType::ImplicitBond,    ColorName::rgbLightBlue1},
        {BondType::UpBond,          ColorName::rgbLightBlue2},
        {BondType::DownBond,        ColorName::rgbLightBlue3}
};

QColor getColor(const BondType &_bondType) {
    auto it = colorMap.find(_bondType);
    if (colorMap.end() == it) {
        return qColor(ColorName::rgbBlack);
    }
    return qColor(it->second);
}