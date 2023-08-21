#pragma once

#include <tuple>
#include <cmath>

template<typename Number>
using point3 = std::tuple<Number, Number, Number>;

using point3f = point3<float>;
using point3i = point3<int>;

template<typename Number>
Number getDistance(const point3<Number> &p1, const point3<Number> &p2) {
    const auto &[x1, y1, z1] = p1;
    const auto &[x2, y2, z2] = p2;
    const auto dx = x1 - x2;
    const auto dy = y1 - y2;
    const auto dz = z1 - z2;
    return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// some templates for basic point3 operation
template<typename Number>
inline point3<Number> &operator-=(point3<Number> &p1, const point3<Number> &p2) {
    auto &[x1, y1, z1] = p1;
    auto &[x2, y2, z2] = p2;
    x1 -= x2;
    y1 -= y2;
    z1 -= z2;
    return p1;
}

template<typename Number>
inline point3<Number> &operator+=(point3<Number> &p1, const point3<Number> &p2) {
    auto &[x1, y1, z1] = p1;
    auto &[x2, y2, z2] = p2;
    x1 += x2;
    y1 += y2;
    z1 += z2;
    return p1;
}

template<typename Number>
inline point3<Number> operator+(const point3<Number> &p1, const point3<Number> &p2) {
    auto &[x1, y1, z1] = p1;
    auto &[x2, y2, z2] = p2;
    return {x1 + x2, y1 + y2, z1 + z2};
}

template<typename Number>
inline point3<Number> operator-(const point3<Number> &p1, const point3<Number> &p2) {
    auto &[x1, y1, z1] = p1;
    auto &[x2, y2, z2] = p2;
    return {x1 - x2, y1 - y2, z1 - z2};
}

template<typename Number>
inline point3<Number> &operator*=(point3<Number> &p1, const Number &k) {
    auto &[x1, y1, z1] = p1;
    x1 *= k;
    y1 *= k;
    z1 *= k;
    return p1;
}

template<typename Number>
inline point3<Number> &operator/=(point3<Number> &p1, const Number &k) {
    auto &[x1, y1, z1] = p1;
    x1 /= k;
    y1 /= k;
    z1 /= k;
    return p1;
}

template<typename Number>
inline point3<Number> operator/(const point3<Number> &p1, const Number &k) {
    auto &[x1, y1, z1] = p1;
    return {x1 / k, y1 / k, z1 / k};
}

template<typename Number>
inline point3<Number> operator*(const point3<Number> &p1, const Number &k) {
    auto &[x1, y1, z1] = p1;
    return {x1 * k, y1 * k, z1 * k};
}


template<typename Number>
inline point3<Number> operator*(const Number &k, const point3<Number> &p1) {
    auto &[x1, y1, z1] = p1;
    return {x1 * k, y1 * k, z1 * k};
}