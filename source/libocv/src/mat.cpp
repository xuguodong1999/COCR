#include "ocv/mat.h"
#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>

static cv::Scalar convertToScalar(const rgb &color) {
    const auto&[r, g, b]=color;
    return {(double) b, (double) g, (double) r};
}

Mat::Mat(const MatChannel &channel, const DataType &dataType, const int &w, const int &h)
        : mChannel(channel), mDataType(dataType), mWidth(w), mHeight(h), holder(nullptr) {
    reset();
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
    holder = std::make_shared<cv::Mat>(
            mHeight, mWidth, getOpenCVDataTypeMacro(), 255);
}

void Mat::drawEllipse(
        const point2f &center, const float &w, const float &h, const float &deg,
        const rgb &color, const int &thickness) {
    if (!holder) { return; }
    auto &canvas = *holder;
    const auto&[cx, cy]=center;
    cv::RotatedRect rect({cx, cy}, cv::Size2f{w, h}, deg);
    cv::ellipse(canvas, rect, convertToScalar(color), thickness, cv::LINE_AA);
}

decltype(Mat::holder) Mat::getHolder() const {
    return holder;
}

unsigned char *Mat::getData() const {
    return holder->data;
}

QImage Mat::toQImage() const {
    // TODO:
    return QImage();
}

Mat::Mat(const QImage &qimage, const MatChannel &channel, const DataType &dataType)
        : mChannel(channel), mDataType(dataType) {
    assert(qimage.isNull());
    QImage qimage2;
    switch (channel) {
        case MatChannel::GRAY:
            qimage2 = qimage.convertToFormat(QImage::Format_Grayscale8);
            break;
        case MatChannel::RGB:
            qimage2 = qimage.convertToFormat(QImage::Format_ARGB32);
            break;
        case MatChannel::RGBA:
        default:
            qimage2 = qimage.convertToFormat(QImage::Format_RGBA64);
            break;
    }
    void *dataPtr = const_cast<uchar *>(qimage2.constBits());
    int step = qimage2.bytesPerLine();
    holder = std::make_shared<cv::Mat>(
            qimage2.height(), qimage2.width(), getOpenCVDataTypeMacro(), dataPtr, step);
}

int Mat::getOpenCVDataTypeMacro() const {
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
    return CV_MAKETYPE(DATA_TYPE_MACRO, (size_t) mChannel);
}

Mat &Mat::operator=(Mat &&mat) noexcept {
    setChannel(mat.getChannel());
    setWidth(mat.getWidth());
    setHeight(mat.getHeight());
    setDataType(mat.getDataType());
    holder = std::move(mat.getHolder());
    return *this;
}

void Mat::clear() {
    // TODO:
}

Mat &Mat::operator=(const Mat &mat) noexcept {
    if (this == &mat) {
        return *this;
    }
    setChannel(mat.getChannel());
    setWidth(mat.getWidth());
    setHeight(mat.getHeight());
    setDataType(mat.getDataType());
    holder = std::make_shared<cv::Mat>();
    *holder = (*mat.holder).clone();
    return *this;
}

Mat Mat::operator()(const recti &box) const {
    const auto&[p0, p1]=box;
    const auto&[x0, y0]=p0;
    const auto&[x1, y1]=p1;
    const int w = std::abs(x0 - x1);
    const int h = std::abs(y0 - y1);
    Mat mat(mChannel, mDataType, w, h);
    mat.holder = std::make_shared<cv::Mat>();
    *(mat.holder) = (*holder)(cv::Rect2i(x0, y0, w, h));
    return mat;
}

void Mat::sync() {
    if (!holder) {
        return;
    }
    mWidth = holder->cols;
    mHeight = holder->rows;
}
