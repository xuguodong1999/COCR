#ifndef _QT_UTIL_HPP_
#define _QT_UTIL_HPP_

#include <QString>
#include <QImage>
#include <QPixmap>

#include <opencv2/core/mat.hpp>

extern const QVector3D axisX, axisY, axisZ, zeroP;
extern const std::vector<QVector3D> tetrahedral;

class FontPixItem {
public:
    static cv::Mat GetFont(const QString &_text, const QString &_fontFamily = "Arial");
};

cv::Mat convertQImageToMat(const QImage &_img);

cv::Mat convertQPixmapToMat(const QPixmap &_img);


#endif//_QT_UTIL_HPP_
