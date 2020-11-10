#ifndef _QT_UTIL_HPP_
#define _QT_UTIL_HPP_

#include <opencv2/core/mat.hpp>
#include <QString>

class FontPixItem {
public:
    static cv::Mat GetFont(const QString &_text, const QString &_fontFamily = "Arial");
};

#endif//_QT_UTIL_HPP_
