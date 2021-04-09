#include "jmol.hpp"
#include <QDebug>
#include <cmath>
#include <iostream>
#include <exception>

using namespace xgd;

std::shared_ptr<JAtom> xgd::JMol::getAtom(const id_type &_aid) {
    auto it = atomMap.find(_aid);
    if (atomMap.end() == it) { return nullptr; }
    return it->second;
}

std::shared_ptr<JBond> xgd::JMol::getBond(const id_type &_bid) {
    auto it = bondMap.find(_bid);
    if (bondMap.end() == it) { return nullptr; }
    return it->second;
}


std::shared_ptr<JAtom> JMol::addAtom(const ElementType &_element, const float &_x, const float &_y) {
    auto atom = std::make_shared<JAtom>(idBase++, _element, _x, _y);
    atomMap[atom->getId()] = atom;
    return atom;
}

std::shared_ptr<JAtom> JMol::addAtom(
        const ElementType &_element, const float &_x, const float &_y, const float &_z) {
    auto atom = addAtom(_element, _x, _y);
    if (!atom)return nullptr;
    atom->set3D(_x, _y, _z);
    return atom;
}


std::shared_ptr<JAtom> JMol::removeAtom(const id_type &_aid) {
    auto atom = getAtom(_aid);
    if (atom) { atomMap.erase(_aid); }
    return atom;
}

std::shared_ptr<JBond> JMol::removeBond(const id_type &_bid) {
    auto bond = getBond(_bid);
    if (bond) { bondMap.erase(_bid); }
    return bond;
}

void JMol::setId(const id_type &_id) {
    id = _id;
}

id_type JMol::getId() {
    return id;
}

JMol::JMol() : id(0), is3DInfoLatest(false), is2DInfoLatest(false), idBase(0) {

}

void JMol::loopAtomVec(std::function<void(JAtom &)> _func) {
    for (auto &[aid, atom]:atomMap) {
        if (atom) { _func(*atom); }
    }
}

void JMol::loopBondVec(std::function<void(JBond &)> _func) {
    for (auto &[bid, bond]:bondMap) {
        if (bond) { _func(*bond); }
    }
}

std::shared_ptr<JAtom> JMol::addAtom(const int &_atomicNumber) {
    auto atom = std::make_shared<JAtom>(idBase++, static_cast<ElementType>(_atomicNumber));
    atomMap[atom->getId()] = atom;
    return atom;
}

void JMol::display() {
    onExtraDataNeeded();
    loopAtomVec([&](JAtom &atom) {
        std::cout << atom.getId() << ":" << atom.getName();
        if (is2DInfoLatest) {
            std::cout << ", 2d(" << atom.x << "," << atom.y << ")";
        }
        if (is3DInfoLatest) {
            std::cout << ", 3d(" << atom.xx << "," << atom.yy << atom.zz << ")";
        }
        std::cout << std::endl;
    });
    loopBondVec([&](JBond &bond) {
        std::cout << bond.getId() << ":" << bond.getBondOrder() << ","
                  << bond.getFrom()->getId() << "-" << bond.getTo()->getId() << std::endl;
    });
}

void JMol::onExtraDataNeeded() {
    // default behavior: do nothing
}

void JMol::norm2D(const float &_w, const float &_h, const float &_x, const float &_y, bool keepRatio) {
    if (!is2DInfoLatest) {
        generate2D();
    }
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    loopAtomVec([&](JAtom &_atom) {
        minx = std::min(minx, _atom.x);
        miny = std::min(miny, _atom.y);
        maxx = std::max(maxx, _atom.x);
        maxy = std::max(maxy, _atom.y);
    });
    float kw = (_w - _x * 2) / (maxx - minx), kh = (_h - _y * 2) / (maxy - miny);
    if (keepRatio) {
        float k = std::min(kw, kh);
        loopAtomVec([&](JAtom &_atom) {
            _atom.x = (_atom.x - minx) * k + _x;
            _atom.y = (_atom.y - miny) * k + _y;
        });
    } else {
        loopAtomVec([&](JAtom &_atom) {
            _atom.x = (_atom.x - minx) * kw + _x;
            _atom.y = (_atom.y - miny) * kh + _y;
        });
    }
}

void JMol::norm3D(const float &_xx, const float &_yy, const float &_zz,
                  const float &_x, const float &_y, const float &_z, bool keepRatio) {
    qDebug() << __FUNCTION__;
    if (!is3DInfoLatest) {
        qDebug() << "generate3D=" << generate3D();
    }
    float minx, miny, minz, maxx, maxy, maxz;
    minx = miny = minz = std::numeric_limits<float>::max();
    maxx = maxy = maxz = std::numeric_limits<float>::lowest();
    loopAtomVec([&](JAtom &_atom) {
        minx = std::min(minx, _atom.xx);
        miny = std::min(miny, _atom.yy);
        minz = std::min(minz, _atom.zz);
        maxx = std::max(maxx, _atom.xx);
        maxy = std::max(maxy, _atom.yy);
        maxz = std::max(maxz, _atom.zz);
    });
    float kx = (_xx - _x * 2) / (maxx - minx), ky = (_yy - _y * 2) / (maxy - miny), kz = (_zz - _z * 2) / (maxz - minz);
    float dx = (minx + maxx) / 2, dy = (miny + maxy) / 2, dz = (minz + maxz) / 2;
    if (keepRatio) {
        float k = std::min(kx, std::min(ky, kz));
        loopAtomVec([&](JAtom &_atom) {
            _atom.xx = (_atom.xx - dx) * k + _x;
            _atom.yy = (_atom.yy - dy) * k + _y;
            _atom.zz = (_atom.zz - dz) * k + _z;
        });
    } else {
        loopAtomVec([&](JAtom &_atom) {
            _atom.xx = (_atom.xx - dx) * kx + _x;
            _atom.yy = (_atom.yy - dy) * ky + _y;
            _atom.zz = (_atom.zz - dz) * kz + _y;
        });
    }
}

float JMol::getAvgBondLength() {
    qDebug() << __FUNCTION__;
    if (bondMap.empty()) { return 0; }
    if (!is3DInfoLatest) {
        qDebug() << "generate3D=" << generate3D();
    }
    float avgBondLength = 0;
    loopBondVec([&](JBond &bond) {
        auto from = bond.getFrom(), to = bond.getTo();
        avgBondLength += std::sqrt(std::pow(from->xx - to->xx, 2) +
                                   std::pow(from->yy - to->yy, 2) +
                                   std::pow(from->zz - to->zz, 2));
    });
    return avgBondLength / bondMap.size();
}

size_t JMol::getBondNum() const {
    return bondMap.size();
}

size_t JMol::getAtomNum() const {
    return atomMap.size();
}

void JMol::set2DInfoLatest(bool _is2DInfoLatest) {
    is2DInfoLatest = _is2DInfoLatest;
}


std::shared_ptr<JBond> JMol::addBond(
        std::shared_ptr<JAtom> _a1, std::shared_ptr<JAtom> _a2, const BondType &_type,
        const float &_offset1, const float &_offset2) {
    auto bond = std::make_shared<JBond>(idBase++, _a1, _a2, _type, _offset1, _offset2);
    bondMap[bond->getId()] = bond;
    return bond;
}

std::shared_ptr<JAtom> JMol::addSuperAtom(
        const std::string &_name, const float &_x0, const float &_y0,
        const float &_x1, const float &_y1) {
    auto atom = std::make_shared<JAtom>(idBase++, _name, _x0, _y0, _x1, _y1);
    atomMap[atom->getId()] = atom;
    return atom;
}


