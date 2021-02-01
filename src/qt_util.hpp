#ifndef _QT_UTIL_HPP_
#define _QT_UTIL_HPP_

#include <QImage>
#include <QPixmap>

#include <opencv2/core/mat.hpp>


cv::Mat convertQImageToMat(const QImage &_img);

cv::Mat convertQPixmapToMat(const QPixmap &_img);


#endif//_QT_UTIL_HPP_
