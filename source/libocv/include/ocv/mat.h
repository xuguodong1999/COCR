#pragma once

#include "els_ocv_export.h"
#include "base/size.h"
#include "base/color_name.h"
#include "base/point2.h"
#include "base/rect.h"
#include <memory>
#include <vector>

#ifdef QT_GUI_LIB

#include <QtGui/QImage>

#endif
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

class ELS_OCV_EXPORT Mat {
    std::shared_ptr<cv::Mat> holder;
    DataType mDataType;
    MatChannel mChannel;
    int mWidth;
    int mHeight;

    int getOpenCVDataTypeMacro() const;

public:
    void clear();

#ifdef QT_GUI_LIB

    QImage toQImage() const;

    Mat(const QImage &qimage, const MatChannel &channel, const DataType &dataType);

#endif

    MatChannel getChannel() const;

    DataType getDataType() const;

    void setDataType(const DataType &dataType);

    void setChannel(const MatChannel &channel);

    Mat(const MatChannel &channel, const DataType &dataType, const int &w, const int &h);

    Mat(const Mat &mat);

    Mat &operator=(Mat &&mat) noexcept;

    Mat &operator=(const Mat &mat) noexcept;

    Mat(Mat &&mat);

    Mat() = delete;

    int getWidth() const;

    unsigned char *getData() const;

    Mat operator()(const recti &box) const;

    int getHeight() const;

    Size<int> getSize() const;

    void setWidth(const int &w);

    void setHeight(const int &h);

    void setSize(const int &w, const int &h);

    void setSize(const Size<int> &size);

    void drawLine(const point2i &from, const point2i &to, const rgb &color, const int &thickness);

    void drawCrossLine(const point2i &center, const int &length, const rgb &color, const int &thickness, bool rotate);

    void drawEllipse(
            const point2f &center, const float &w, const float &h, const float &deg,
            const rgb &color, const int &thickness);

    void drawRectangle(const rectf &box, const rgb &color, const int &thickness);

    void drawFill(const std::vector<point2i> &pts, const rgb &color);

    void drawText(
            const std::string &text, const point2i &org,
            const float &scale, const rgb &color, const int &thickness);

    void drawImage(const Mat &mat, const recti &pos);

    void reset();

    void sync();

    decltype(holder) getHolder() const;

    std::vector<unsigned char> toBuffer() const;

    bool saveJPG(const std::string &fileName) const;

    void display(const std::string &name = "") const;
};