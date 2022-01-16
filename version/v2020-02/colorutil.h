#ifndef COLORUTIL_H
#define COLORUTIL_H

#include "cocr.h"

#include <vector>

using std::vector;

#include <opencv2/opencv.hpp>

using cv::Scalar;
using cv::Mat;

#include <QImage>

class COCR_EXPORTS ColorUtil {
public:
    ColorUtil();

    static Scalar getColor(int index, int total);

    static Mat qimage2mat(const QImage &image);

private:
    static vector<vector<float>> colorTable;

};

#endif // COLORUTIL_H
