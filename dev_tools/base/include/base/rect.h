#pragma once

#include "base/point2.h"
#include "base/size.h"
#include <tuple>
#include <cmath>

/**
 * store as [[x0, y0], [x1, y1]]
 */
template<typename Number>
using rect = std::pair<point2<Number>, point2<Number>>;

using rectf = rect<float>;
using recti = rect<int>;

template<typename Number>
const Size<Number> getSize(const rect<Number> &_rect) {
    const auto&[lt, br]=_rect;
    const auto&[x0, y0]=lt;
    const auto&[x1, y1]=br;
    return {std::abs(x1 - x0), std::abs(y1 - y0)};
}

template<typename Number>
const point2f getCenter(const rect<Number> &_rect) {
    const auto&[lt, br]=_rect;
    const auto&[x0, y0]=lt;
    const auto&[x1, y1]=br;
    return {std::round((x1 + x0) / 2.0), std::round((y1 + y0) / 2.0)};
}