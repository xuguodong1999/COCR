#ifndef _FONT_PIX_ITEM_HPP_
#define _FONT_PIX_ITEM_HPP_

#include <QString>
#include <opencv2/core/mat.hpp>

class FontPixItem {
public:
    static cv::Mat GetFont(const QString &_text, const QString &_fontFamily = "Arial");
};

#endif//_FONT_PIX_ITEM_HPP_