#include "ocv_wrapper/mat.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

static cv::Scalar convertToScalar(const rgb &color) {
    const auto&[r, g, b]=color;
    return {(double) b, (double) g, (double) r};
}

Mat::Mat(const MatChannel &channel, const DataType &dataType, const int &w, const int &h)
        : mChannel(channel), mDataType(dataType), mWidth(w), mHeight(h), holder(nullptr) {
}

Mat::Mat(Mat &&mat) : Mat(mat.mChannel, mat.mDataType, mat.mWidth, mat.mHeight) {
    holder = std::move(mat.holder);
}

Mat::Mat(const Mat &mat) : Mat(mat.mChannel, mat.mDataType, mat.mWidth, mat.mHeight) {
    if (mat.holder) {
        holder = std::make_shared<cv::Mat>();
        *holder = *mat.holder;
    }
}

MatChannel Mat::getChannel() const {
    return mChannel;
}

void Mat::setChannel(const MatChannel &channel) {
    mChannel = channel;
}

int Mat::getWidth() const {
    return mWidth;
}

int Mat::getHeight() const {
    return mHeight;
}

Size<int> Mat::getSize() const {
    return {mWidth, mHeight};
}

void Mat::setWidth(const int &w) {
    mWidth = w;
}

void Mat::setHeight(const int &h) {
    mHeight = h;
}

void Mat::setSize(const int &w, const int &h) {
    setSize({w, h});
}

void Mat::setSize(const Size<int> &size) {
    std::tie(mWidth, mHeight) = size;
}

DataType Mat::getDataType() const {
    return mDataType;
}

void Mat::setDataType(const DataType &dataType) {
    mDataType = dataType;
}

void Mat::drawLine(const point2i &from, const point2i &to, const rgb &color, const int &thickness) {
    if (!holder) { return; }
    auto &canvas = *holder;
    const auto&[x0, y0]=from;
    const auto&[x1, y1]=to;

    cv::line(canvas, {x0, y0}, {x1, y1}, convertToScalar(color), thickness, cv::LINE_AA);
}

void Mat::drawFill(const std::vector<point2i> &pts, const rgb &color) {
    if (!holder) { return; }
    auto &canvas = *holder;
    const auto&[r, g, b]=color;
    std::vector<cv::Point2i> cvPts(pts.size());
    for (size_t i = 0; i < pts.size(); i++) {
        const auto&[x, y]=pts[i];
        cvPts[i] = {x, y};
    }
    cv::fillPoly(canvas, cvPts, convertToScalar(color), cv::LINE_AA, 0);
}

void Mat::reset() {
    int DATA_TYPE_MACRO;
    switch (mDataType) {
        case DataType::FLOAT32:
            DATA_TYPE_MACRO = CV_32F;
            break;
        case DataType::UINT8:
        default:
            DATA_TYPE_MACRO = CV_8U;
            break;
    }
    holder = std::make_shared<cv::Mat>(
            mHeight, mWidth, CV_MAKETYPE(DATA_TYPE_MACRO, (size_t) mChannel), 0);
}

void Mat::drawEllipse(
        const point2f &center, const float &w, const float &h, const float &deg,
        const rgb &color, const int &thickness) {
    if (!holder) { return; }
    auto &canvas = *holder;
    const auto&[cx, cy]=center;
    cv::RotatedRect rect({cx, cy}, cv::Size(w, h), deg);
    cv::ellipse(canvas, rect, convertToScalar(color), thickness, cv::LINE_AA);
}

