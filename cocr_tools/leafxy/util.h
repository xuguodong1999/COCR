#pragma once

#include "chem/jbond.hpp"

#include <opencv2/core/types.hpp>

#include <QImage>
#include <QPixmap>

namespace cocr {
    cv::Mat convertQImageToMat(const QImage &_img);

    cv::Mat convertQPixmapToMat(const QPixmap &_img);

    QImage binaryAlphaImage(cv::Mat &src);

    QString getBondName(const cocr::JBond &bond);

    QColor getColor(const ColorName &color);
}