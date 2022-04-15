#pragma once

#include <tuple>
#include <cmath>

template<typename Number>
using point2 = std::pair<Number, Number>;

using point2f = point2<float>;
using point2i = point2<int>;

template<typename Number>
double norm(const point2<Number> &p) {
    const auto&[x, y]=p;
    return std::sqrt(x * x + y * y);
}

template<typename Number>
Number getDistance(const point2<Number> &p1, const point2<Number> &p2) {
    const auto&[x1, y1]=p1;
    const auto&[x2, y2]=p2;
    const auto dx = x1 - x2;
    const auto dy = y1 - y2;
    return std::sqrt(dx * dx + dy * dy);
}

// some templates for basic point2 operation
template<typename Number, typename Number2>
inline point2<Number> &operator-=(point2<Number> &p1, const point2<Number2> &p2) {
    auto&[x1, y1]=p1;
    auto&[x2, y2]=p2;
    x1 -= x2;
    y1 -= y2;
    return p1;
}

template<typename Number, typename Number2>
inline point2<Number> &operator+=(point2<Number> &p1, const point2<Number2> &p2) {
    auto&[x1, y1]=p1;
    auto&[x2, y2]=p2;
    x1 += x2;
    y1 += y2;
    return p1;
}

template<typename Number>
inline point2<Number> operator+(const point2<Number> &p1, const point2<Number> &p2) {
    auto&[x1, y1]=p1;
    auto&[x2, y2]=p2;
    return {x1 + x2, y1 + y2};
}

template<typename Number>
inline point2<Number> operator-(const point2<Number> &p1, const point2<Number> &p2) {
    auto&[x1, y1]=p1;
    auto&[x2, y2]=p2;
    return {x1 - x2, y1 - y2};
}

template<typename Number, typename ConstType>
inline point2<Number> &operator*=(point2<Number> &p1, const ConstType &k) {
    auto&[x1, y1]=p1;
    x1 *= k;
    y1 *= k;
    return p1;
}

template<typename Number, typename ConstType>
inline point2<Number> &operator/=(point2<Number> &p1, const ConstType &k) {
    auto&[x1, y1]=p1;
    x1 /= k;
    y1 /= k;
    return p1;
}

template<typename Number, typename ConstType>
inline point2<Number> operator/(const point2<Number> &p1, const ConstType &k) {
    auto&[x1, y1]=p1;
    return {x1 / k, y1 / k};
}

template<typename Number, typename ConstType>
inline point2<Number> operator*(const point2<Number> &p1, const ConstType &k) {
    auto&[x1, y1]=p1;
    return {x1 * k, y1 * k};
}


template<typename Number>
inline point2<Number> operator*(const Number &k, const point2<Number> &p1) {
    auto&[x1, y1]=p1;
    return {x1 * k, y1 * k};
}