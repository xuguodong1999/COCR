#include "XGD/Chem/Atom.h"
#include "XGD/Base/ColorName.h"

#include <iostream>
#include <algorithm>

const std::string &Atom::getElementName() const {
    return ElementUtil::convertElementTypeToString(elementType);
}

void Atom::setElementType(const ElementType &_elementType) {
    elementType = _elementType;
}

void Atom::setElementType(const std::string &_elementName) {
    elementType = ElementUtil::convertStringToElementType(_elementName);
}

void Atom::setElementType(const size_t &_atomicNumber) {
    elementType = static_cast<ElementType>(_atomicNumber);
}

const ElementType &Atom::getElementType() const {
    return elementType;
}

size_t Atom::getAtomicNumber() const {
    return static_cast<size_t>(elementType);
}


Atom::Atom() : xx(-1), yy(-1), zz(-1) {

}

Atom::Atom(const id_type &_id, const size_t &_atomicNumber) : xx(-1), yy(-1), zz(-1), id(_id) {
    setElementType(_atomicNumber);
}

const AtomStereo &Atom::getAtomStereo() const {
    return atomStereo;
}

void Atom::setAtomStereo(const AtomStereo &_atomStereo) {
    atomStereo = _atomStereo;
}

bool Atom::isCoord2dEmbedded() const {
    return std::fabs(xx + 1) > 0.001 || std::fabs(yy + 1) > 0.001;
}

void Atom::setCoord2d(const float &_x, const float &_y) {
    x = xx = _x;
    y = yy = _y;
    zz = -1;
}

const std::string &Atom::getName() const {
    if (isSuperAtom()) {
        return name;
    }
    return ElementUtil::convertElementTypeToString(elementType);
}

const ElementType &Atom::getType() const {
    return elementType;
}

void Atom::set2D(const float &_x, const float &_y) {
    x = x0 = x1 = _x;
    y = y0 = y1 = _y;
}

void Atom::set3D(const float &_x, const float &_y, const float &_z) {
    xx = _x;
    yy = _y;
    zz = _z;
}

int Atom::getCharge() const {
    return charge;
}


Atom::Atom(const id_type &_id, const ElementType &_element, const float &_x, const float &_y)
        : id(_id), mIsImplicit(false), elementType(_element), x(_x), y(_y),
          x0(_x), y0(_y), x1(_x), y1(_y), xx(0), yy(0), zz(0), charge(0), mIsLeftToRight(false) {
}

Atom::Atom(const id_type &_id, const std::string &_name,
           const float &_x0, const float &_y0, const float &_x1, const float &_y1)
        : id(_id), mIsImplicit(false), elementType(ElementType::SA), x((_x0 + _x1) / 2), y((_y0 + _y1) / 2),
          x0(_x0), y0(_y0), x1(_x1), y1(_y1), xx(0), yy(0), zz(0), charge(0), name(_name), mIsLeftToRight(false) {

}

float Atom::getMass() const {
    return ElementUtil::GetElementMass(elementType);
}

float Atom::getElectronNeg() const {
    return ElementUtil::GetElementElectron(elementType);
}

float Atom::getRadius() const {
    auto result = ElementUtil::GetElementRadius(elementType);
    if (!result) {
        return sDefaultRadius;
    }
    return result.value();
}

float Atom::getDefaultRadius() {
    return sDefaultRadius;
}


bool Atom::isImplicit() const {
    return mIsImplicit;
}

void Atom::setImplicit(bool _isImplicit) {
    mIsImplicit = _isImplicit;
}

void Atom::setId(const id_type &_id) {
    id = _id;
}

id_type Atom::getId() {
    return id;
}

bool Atom::isSuperAtom() const {
    return ElementType::SA == elementType;
}

void Atom::setCharge(int charge) {
    Atom::charge = charge;
}

void Atom::setType(const ElementType &_type) {
    elementType = _type;
}

bool Atom::isLeftToRight() const {
    return mIsLeftToRight;
}

void Atom::setIsLeftToRight(bool isLeftToRight) {
    if (!mIsLeftToRight) { mIsLeftToRight = isLeftToRight; }
}

int Atom::getCommonNebNum() const {
    auto result = ElementUtil::GetElementNebNum(elementType);
    if (!result) {
        return 4;
    }
    return result.value();
}

std::vector<std::pair<float, std::shared_ptr<Bond>>> &Atom::getSaBonds() {
    return saBonds;
}

void Atom::insertSuperAtomBonds(std::shared_ptr<Bond> _bond, const float &_offset) {
    saBonds.emplace_back(_offset, std::move(_bond));
}

void Atom::clearSABonds() {
    saBonds.clear();
}

