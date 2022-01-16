#include "colorutil.h"

vector<vector<float>> ColorUtil::colorTable = {    // R G B A
        {1.00000, 0.75294, 0.79608, 1.00000},    // Pink
        {0.85490, 0.64706, 0.12549, 1.00000},    // Golden Rod
        {0.56078, 0.56078, 1.00000, 1.00000},    // Sky Blue
        {0.94118, 0.00000, 0.00000, 1.00000},    // Red
        {1.00000, 0.78431, 0.00000, 1.00000},    // Yellow
        {0.62745, 0.12549, 0.94118, 1.00000},    // Purple
        {1.00000, 0.64706, 0.00000, 1.00000},    // Orange
        {0.00000, 0.00000, 1.00000, 1.00000},    // Blue
        {0.50196, 0.50196, 0.56471, 1.00000},    // Dark Grey
        {0.64706, 0.16471, 0.16471, 1.00000},    // Brown
        {1.00000, 0.07843, 0.57647, 1.00000},    // Deep Pink
        {0.00000, 1.00000, 0.00000, 1.00000},    // Green
        {0.69804, 0.13333, 0.13333, 1.00000},    // Fire Brick
        {0.13333, 0.54510, 0.13333, 1.00000},    // Forest Green
        {0.78431, 0.78431, 0.78431, 1.00000},    // Light Grey
        {1.00000, 1.00000, 1.00000, 1.00000}    // White
};

Scalar ColorUtil::getColor(int index, int total) {
    const vector<float> &color = colorTable[index % colorTable.size()];
    return 255.0 * Scalar(color[2], color[1], color[0], color[3]);
}

Mat ColorUtil::qimage2mat(const QImage &image) {
    /** 代码出处： https://blog.csdn.net/weixin_38621214/article/details/85311048
    */
    switch (image.format()) {
        case QImage::Format_ARGB32:
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32_Premultiplied:
            return Mat(image.height(), image.width(), CV_8UC4,
                       (void *) image.constBits(), image.bytesPerLine());
        case QImage::Format_RGB888: {
            Mat mat = Mat(image.height(), image.width(), CV_8UC3,
                          (void *) image.constBits(), image.bytesPerLine());
            cvtColor(mat, mat, cv::COLOR_BGR2RGB);
            return mat;
        }
        case QImage::Format_Indexed8:
            return Mat(image.height(), image.width(), CV_8UC1,
                       (void *) image.constBits(), image.bytesPerLine());
        default:
            assert(false);
            return Mat();
    }
}

ColorUtil::ColorUtil() {

}
