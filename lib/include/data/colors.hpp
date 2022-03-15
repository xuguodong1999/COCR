#ifndef _COLORS_HPP_
#define _COLORS_HPP_

#include <vector>
#include <tuple>

#ifdef QT_GUI_LIB

#include <QColor>

#endif

#include <opencv2/core/types.hpp>


#ifdef QT_GUI_LIB

QColor getQColor(const ColorName &_colorName);

#endif

cv::Scalar getScalar(const ColorName &_colorName);

#endif//_COLORS_HPP_
