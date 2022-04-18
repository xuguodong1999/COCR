#include "ocv/mat.h"
#include "base/std_util.h"

#include <opencv2/core/mat.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#if not defined(Q_OS_WASM)
#include <opencv2/imgcodecs.hpp>
#endif
#include <opencv2/highgui.hpp>

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

#ifdef QT_GUI_LIB

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

#endif

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

void Mat::drawCrossLine(const point2i &center, const int &length, const rgb &color, const int &thickness, bool rotate) {
    if (!holder) { return; }
    auto &canvas = *holder;
    const auto&[x, y]=center;
    int xx[4], yy[4];
    if (rotate) {
        xx[0] = xx[3] = x - length;
        xx[1] = xx[2] = x + length;
        yy[0] = yy[2] = y - length;
        yy[1] = yy[3] = y + length;
    } else {
        xx[0] = xx[1] = x;
        xx[2] = x - length;
        xx[3] = x + length;
        yy[2] = yy[3] = y;
        yy[0] = y - length;
        yy[1] = y + length;
    }
    cv::line(canvas, cv::Point(xx[0], yy[0]), cv::Point(xx[1], yy[1]),
             convertToScalar(color), thickness, cv::LINE_AA);
    cv::line(canvas, cv::Point(xx[2], yy[2]), cv::Point(xx[3], yy[3]),
             convertToScalar(color), thickness, cv::LINE_AA);
}

std::vector<unsigned char> Mat::toBuffer() const {
    std::vector<uchar> buffer;
    if (!holder) { return buffer; }
    auto &canvas = *holder;
    cv::imencode(".jpg", *holder, buffer, {cv::IMWRITE_JPEG_QUALITY, 100});
    return buffer;
}

bool Mat::saveJPG(const std::string &fileName) const {
#if not defined(Q_OS_WASM)
    // 60 + StdUtil::randInt() % 40 for crnn test sample, yolo all
    // 70 + StdUtil::randInt() % 30 for crnn train sample
    return cv::imwrite(fileName, *holder, {cv::IMWRITE_JPEG_QUALITY, 60 + StdUtil::randInt() % 40});
#else
    return false;
#endif
}

void Mat::drawText(
        const std::string &text, const point2i &org, const float &scale, const rgb &color, const int &thickness) {
    if (!holder) { return; }
    auto &canvas = *holder;
    const auto&[x, y]=org;
    static const std::vector<int> fontChoices{
            cv::FONT_HERSHEY_SIMPLEX, cv::FONT_HERSHEY_DUPLEX, cv::FONT_HERSHEY_COMPLEX,
            cv::FONT_HERSHEY_TRIPLEX};
    cv::putText(
            canvas, text, cv::Point{x, y},
            StdUtil::randSelect(fontChoices), 1.5, convertToScalar(color),
            thickness, cv::LINE_AA, false);
}

void Mat::display(const std::string &name) const {
    cv::imshow(name, *holder);
    cv::waitKey(0);
}

void Mat::drawImage(const Mat &mat, const recti &pos) {
    const auto &src = *(mat.getHolder());
    auto &dst = *holder;
    const auto&[tl, br]=pos;
    src.copyTo(dst(cv::Rect2i(cv::Point2i{tl.first, tl.second}, cv::Point2i{br.first, br.second})));
}

void Mat::drawRectangle(const rectf &box, const rgb &color, const int &thickness) {
    auto &dst = *holder;
    const auto&[tl, br]=box;
    const auto&[x0, y0]=tl;
    const auto&[x1, y1]=br;
    cv::rectangle(dst, cv::Point{(int) x0, (int) y0}, cv::Point{(int) x1, (int) y1},
                  convertToScalar(color), thickness, cv::LINE_AA, 0);
}
