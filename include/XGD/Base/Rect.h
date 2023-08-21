#pragma once

#include "XGD/Base/Vec2.h"
#include "XGD/Base/Size.h"
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
Size<Number> getSize(const rect<Number> &_rect) {
    const auto &[lt, br] = _rect;
    const auto &[x0, y0] = lt;
    const auto &[x1, y1] = br;
    return {std::abs(x1 - x0), std::abs(y1 - y0)};
}

template<typename Number>
Number getAvgWidth(const rect<Number> &_rect) {
    const auto &[lt, br] = _rect;
    const auto &[x0, y0] = lt;
    const auto &[x1, y1] = br;
    return std::round((std::abs(x1 - x0) + std::abs(y1 - y0)) / 2.0);
}

template<typename Number>
point2f getCenter(const rect<Number> &_rect) {
    const auto &[lt, br] = _rect;
    const auto &[x0, y0] = lt;
    const auto &[x1, y1] = br;
    return {(float) std::round((x1 + x0) / 2.0), (float) std::round((y1 + y0) / 2.0)};
}