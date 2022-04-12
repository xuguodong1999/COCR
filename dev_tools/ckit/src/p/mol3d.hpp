#pragma once

#include "chem/mol_holder.hpp"
#include <unordered_map>

template<typename Number>
using point3 = std::tuple<Number, Number, Number>;

using point3f = point3<float>;

class COCR_CHEM_EXPORT Mol3D : public MolHolder {
protected:
    //3D 原子坐标
    std::unordered_map<size_t, point3f> atomPosMap3D;
public:
    Mol3D(std::shared_ptr<JMol> _mol);

    void normAtomPosMap3D(const float &_targetBondLength);

    const point3f &getAtomPos3DById(const size_t &_aid);

    void setAtomPos3DById(const size_t &_aid, const float &_x, const float &_y, const float &_z);

    bool calcCoord3D_addHs();
};

// some templates for basic point3 operation
template<typename Number>
inline point3<Number> &operator-=(point3<Number> &p1, const point3<Number> &p2) {
    auto&[x1, y1, z1]=p1;
    auto&[x2, y2, z2]=p2;
    x1 -= x2;
    y1 -= y2;
    z1 -= z2;
    return p1;
}

template<typename Number>
inline point3<Number> &operator+=(point3<Number> &p1, const point3<Number> &p2) {
    auto&[x1, y1, z1]=p1;
    auto&[x2, y2, z2]=p2;
    x1 += x2;
    y1 += y2;
    z1 += z2;
    return p1;
}

template<typename Number>
inline point3<Number> operator+(const point3<Number> &p1, const point3<Number> &p2) {
    auto&[x1, y1, z1]=p1;
    auto&[x2, y2, z2]=p2;
    return {x1 + x2, y1 + y2, z1 + z2};
}

template<typename Number>
inline point3<Number> operator-(const point3<Number> &p1, const point3<Number> &p2) {
    auto&[x1, y1, z1]=p1;
    auto&[x2, y2, z2]=p2;
    return {x1 - x2, y1 - y2, z1 - z2};
}

template<typename Number>
inline point3<Number> &operator*=(point3<Number> &p1, const Number &k) {
    auto&[x1, y1, z1]=p1;
    x1 *= k;
    y1 *= k;
    z1 *= k;
    return p1;
}

template<typename Number>
inline point3<Number> &operator/=(point3<Number> &p1, const Number &k) {
    auto&[x1, y1, z1]=p1;
    x1 /= k;
    y1 /= k;
    z1 /= k;
    return p1;
}

template<typename Number>
inline point3<Number> operator/(const point3<Number> &p1, const Number &k) {
    auto&[x1, y1, z1]=p1;
    return {x1 / k, y1 / k, z1 / k};
}

template<typename Number>
inline point3<Number> operator*(const point3<Number> &p1, const Number &k) {
    auto&[x1, y1, z1]=p1;
    return {x1 * k, y1 * k, z1 * k};
}


template<typename Number>
inline point3<Number> operator*(const Number &k, const point3<Number> &p1) {
    auto&[x1, y1, z1]=p1;
    return {x1 * k, y1 * k, z1 * k};
}