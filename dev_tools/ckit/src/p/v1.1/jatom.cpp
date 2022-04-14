#include "jatom.h"
#include "base/cocr_types.h"
#include <vector>
#include <string>
#include <unordered_map>

const std::string &v1_1::JAtom::getName() const {
    if (isSuperAtom()) {
        return name;
    }
    return ElementUtil::convertElementTypeToString(type);
}

const ElementType &v1_1::JAtom::getType() const {
    return type;
}

void v1_1::JAtom::set2D(const float &_x, const float &_y) {
    x = x0 = x1 = _x;
    y = y0 = y1 = _y;
}

void v1_1::JAtom::set3D(const float &_x, const float &_y, const float &_z) {
    xx = _x;
    yy = _y;
    zz = _z;
}

int v1_1::JAtom::getCharge() const {
    return charge;
}


v1_1::JAtom::JAtom(const size_t &_id, const ElementType &_element, const float &_x, const float &_y)
        : id(_id), mIsImplicit(false), type(_element), x(_x), y(_y),
          x0(_x), y0(_y), x1(_x), y1(_y), xx(0), yy(0), zz(0), charge(0), mIsLeftToRight(false) {
}

v1_1::JAtom::JAtom(const size_t &_id, const std::string &_name,
                   const float &_x0, const float &_y0, const float &_x1, const float &_y1)
        : id(_id), mIsImplicit(false), type(ElementType::SA), x((_x0 + _x1) / 2), y((_y0 + _y1) / 2),
          x0(_x0), y0(_y0), x1(_x1), y1(_y1), xx(0), yy(0), zz(0), charge(0), name(_name), mIsLeftToRight(false) {

}

int v1_1::JAtom::getAtomicNumber() const {
    return static_cast<size_t>(type);
}

float v1_1::JAtom::getMass() const {
    return ElementUtil::GetElementMass(type);
}

float v1_1::JAtom::getElectronNeg() const {
    return ElementUtil::GetElementElectron(type);
}

float v1_1::JAtom::getRadius() const {
    auto result = ElementUtil::GetElementRadius(type);
    if (!result) {
        return sDefaultRadius;
    }
    return result.value();
}

float v1_1::JAtom::getDefaultRadius() {
    return sDefaultRadius;
}


bool v1_1::JAtom::isImplicit() const {
    return mIsImplicit;
}

void v1_1::JAtom::setImplicit(bool _isImplicit) {
    mIsImplicit = _isImplicit;
}

void v1_1::JAtom::setId(const id_type &_id) {
    id = _id;
}

id_type v1_1::JAtom::getId() {
    return id;
}

bool v1_1::JAtom::isSuperAtom() const {
    return ElementType::SA == type;
}

void v1_1::JAtom::setCharge(int charge) {
    JAtom::charge = charge;
}

void v1_1::JAtom::setType(const ElementType &_type) {
    type = _type;
}

bool v1_1::JAtom::isLeftToRight() const {
    return mIsLeftToRight;
}

void v1_1::JAtom::setIsLeftToRight(bool isLeftToRight) {
    if (!mIsLeftToRight) { mIsLeftToRight = isLeftToRight; }
}

int v1_1::JAtom::getCommonNebNum() const {
    auto result = ElementUtil::GetElementNebNum(type);
    if (!result) {
        return 4;
    }
    return result.value();
}

std::vector<std::pair<float, std::shared_ptr<v1_1::JBond>>> &v1_1::JAtom::getSaBonds() {
    return saBonds;
}

void v1_1::JAtom::insertSuperAtomBonds(std::shared_ptr<JBond> _bond, const float &_offset) {
    saBonds.push_back(std::make_pair(_offset, std::move(_bond)));
}

void v1_1::JAtom::clearSABonds() {
    saBonds.clear();
}


