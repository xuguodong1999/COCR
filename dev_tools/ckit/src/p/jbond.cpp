#include "chem/jbond.hpp"
#include "chem/jatom.hpp"
#include "base/cocr_types.hpp"
#include <stdexcept>

using namespace cocr;

JBond::JBond(const id_type &_id, std::shared_ptr<JAtom> _from, std::shared_ptr<JAtom> _to, const BondType &_type,
             const float &_offset1, const float &_offset2)
        : id(_id), type(_type), from(std::move(_from)), to(std::move(_to)), offset1(_offset1), offset2(_offset2) {
}

std::shared_ptr<JAtom> JBond::getFrom() const {
    return from;
}

std::shared_ptr<JAtom> JBond::getTo() const {
    return to;
}

id_type JBond::getId() const {
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

float JBond::getFromOffset() const {
    return offset1;
}

float JBond::getToOffset() const {
    return offset2;
}

void JBond::setFrom(const std::shared_ptr<JAtom> &from) {
    JBond::from = from;
}

void JBond::setTo(const std::shared_ptr<JAtom> &to) {
    JBond::to = to;
}

std::string JBond::getName() const {
    switch (type) {
        case BondType::SingleBond:
            return "Single";
        case BondType::DoubleBond:
            return "Double";
        case BondType::TripleBond:
            return "Triple";
        case BondType::DelocalizedBond:
            return "Delocalized";
        case BondType::ImplicitBond:
            return "Implicit";
        case BondType::UpBond:
            return "UpBond";
        case BondType::DownBond:
            return "Down";
        default:
            return "Unknown";
    }
}

static std::unordered_map<BondType, ColorName> colorMap = {
        {BondType::SingleBond,      ColorName::rgbLightGoldenrod1},
        {BondType::DoubleBond,      ColorName::rgbDarkGoldenrod1},
        {BondType::TripleBond,      ColorName::rgbDarkMagenta},
        {BondType::DelocalizedBond, ColorName::rgbYellow},
        {BondType::ImplicitBond,    ColorName::rgbLightGoldenrod2},
        {BondType::UpBond,          ColorName::rgbLightGoldenrod3},
        {BondType::DownBond,        ColorName::rgbLightGoldenrod4}
};

ColorName cocr::getBondColor(const BondType &_bondType) {
    auto it = colorMap.find(_bondType);
    if (colorMap.end() == it) {
        return ColorName::rgbBlack;
    }
    return it->second;
}