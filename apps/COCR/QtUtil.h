#pragma once

#include "XGD/Chem/Bond.h"

//#include <opencv2/core/types.hpp>

#include <QtGui/QImage>
#include <QtGui/QPixmap>

//cv::Mat convertQImageToMat(const QImage &_img);

//cv::Mat convertQPixmapToMat(const QPixmap &_img);

//QImage binaryAlphaImage(cv::Mat &src);

QString getBondName(const Bond &bond);

QColor getColor(const ColorName &color);
