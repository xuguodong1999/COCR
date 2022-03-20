#pragma once

#include "base/cocr_types.hpp"

#include <vector>
#include <tuple>

#include <QColor>
#include <opencv2/core/types.hpp>

inline QColor getQColor(const ColorName &_colorName) {
    const auto&[r, g, b]=
    predefinedColors[(size_t) _colorName];
    return {r, g, b};
}


inline cv::Scalar_<unsigned char> getScalar(const ColorName &_colorName) {
    const auto&[r, g, b]=
    predefinedColors[(size_t) _colorName];
    return {b, g, r};
}