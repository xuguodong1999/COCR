#pragma once

#include "els_ocv_wrapper_export.h"
#include "base/size.h"
#include "base/color_name.h"
#include "base/point2.h"
#include <memory>
#include <vector>

namespace cv {
    class Mat;
}

enum class MatChannel : unsigned char {
    GRAY = 1,
    RGB = 3,
    RGBA = 4,
};
enum class DataType : unsigned char {
    FLOAT32,
    UINT8,
};

class ELS_OCV_WRAPPER_EXPORT Mat {
    std::shared_ptr<cv::Mat> holder;
    DataType mDataType;
    MatChannel mChannel;
    int mWidth;
    int mHeight;
public:
    MatChannel getChannel() const;

    DataType getDataType() const;

    void setDataType(const DataType &dataType);

    void setChannel(const MatChannel &channel);

    Mat(const MatChannel &channel, const DataType &dataType, const int &w, const int &h);

    Mat(const Mat &mat);

    Mat(Mat &&mat);

    Mat() = delete;

    int getWidth() const;

    int getHeight() const;

    Size<int> getSize() const;

    void setWidth(const int &w);

    void setHeight(const int &h);

    void setSize(const int &w, const int &h);

    void setSize(const Size<int> &size);

    void drawLine(const point2i &from, const point2i &to, const rgb &color, const int &thickness);

    void drawEllipse(
            const point2f &center, const float &w, const float &h, const float &deg,
            const rgb &color, const int &thickness);

    void drawFill(const std::vector<point2i> &pts, const rgb &color);

    void reset();
};