#pragma once

#include <boost/rational.hpp>
#include <boost/integer/common_factor.hpp>
#include <cmath>

using frac = boost::rational<int>;

template<typename Out, typename T>
requires std::is_integral_v<Out>
Out fracToBase(const boost::rational<T> &f) {
    return static_cast<Out>(std::round(static_cast<double>(f.numerator()) / f.denominator()));
}

template<typename Out, typename T>
requires std::is_floating_point_v<Out>
Out fracToBase(const boost::rational<T> &f) {
    return static_cast<Out>(f.numerator()) / f.denominator();
}
