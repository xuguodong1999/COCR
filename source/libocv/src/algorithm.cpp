#include "ocv/algorithm.h"

#include "base/std_util.h"

#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>

#if not defined(Q_OS_WASM)
#include <opencv2/imgcodecs.hpp>
#endif
#include <opencv2/highgui.hpp>

#include <QFont>
#include <QPainter>
#include <QTextDocument>

#include <optional>

static cv::Scalar convertToScalar(const rgb &color) {
    const auto&[r, g, b]=color;
    return {(double) b, (double) g, (double) r};
}

static cv::Scalar convertToScalar(const ColorName &color) {
    const auto&[r, g, b]=ColorUtil::GetRGB(color);
    return {(double) b, (double) g, (double) r};
}

static void sync(std::vector<cv::Point2f> &dst, const std::vector<point2f> &src) {
    dst.resize(src.size());
    for (size_t i = 0; i < src.size(); i++) {
        const auto&[x, y]=src[i];
        dst[i] = {x, y};
    }
}

void AffineHelper::calc(const std::vector<point2f> &from, const std::vector<point2f> &to) {
    std::vector<cv::Point2f> _from, _to;
    sync(_from, from);
    sync(_to, to);
    // 2x3 仿射变换矩阵
    auto matrix = cv::getAffineTransform(_from.data(), _to.data());
    affine = std::make_shared<cv::Mat>();
    *affine = matrix;
}

point2f AffineHelper::transform(const point2f &p) {
    const auto&[x, y]=p;
    cv::Mat pm = (*affine) * cv::Mat({(double) x, (double) y, 1.});
    return {pm.at<double>(0, 0), pm.at<double>(1, 0)};
}

std::vector<point2f> CvUtil::GetMinAreaRect(const std::vector<point2f> &input) {
    std::vector<cv::Point2f> dst(4), _input;
    sync(_input, input);
    cv::minAreaRect(_input).points(dst.data());
    std::vector<point2f> result(dst.size());
    for (size_t i = 0; i < dst.size(); i++) {
        const auto &p = dst[i];
        result[i] = {p.x, p.y};
    }
    return result;
}

static int sMinSize = 5;
static float sLineThresh = 2;

template<typename _Tp>
inline _Tp getScalarSum(const cv::Scalar_<_Tp> &_scalar) {
    const auto val = _scalar.val;
    _Tp sum = 0;
    for (size_t i = 0; i < 4; i++) {
        sum += val[i];
    }
    return sum;
}

std::pair<cv::Point2f, cv::Point2f> getLineFromTo(const cv::Mat &mat, const cv::Rect2f &box);

std::pair<cv::Point2f, cv::Point2f> getWedgeFromTo(const cv::Mat &mat, const cv::Rect2f &box);

std::pair<point2f, point2f> CvUtil::GetLineFromTo(const Mat &mat, const rectf &box) {
    auto matPtr = mat.getHolder();
    assert(matPtr);
    const auto &cvMat = *matPtr;
    const auto&[p0, p1]=box;
    const auto&[x0, y0]=p0;
    const auto&[x1, y1]=p1;
    const auto[from, to]=getLineFromTo(cvMat, cv::Rect2f(cv::Point2f{x0, y0}, cv::Point2f{x1, y1}));
    return std::make_pair(point2f{from.x, from.y}, point2f{to.x, to.y});
}

std::pair<point2f, point2f> CvUtil::GetWedgeFromTo(const Mat &mat, const rectf &box) {
    auto matPtr = mat.getHolder();
    assert(matPtr);
    const auto &cvMat = *matPtr;
    const auto&[p0, p1]=box;
    const auto&[x0, y0]=p0;
    const auto&[x1, y1]=p1;
    const auto[from, to]=getWedgeFromTo(cvMat, cv::Rect2f(cv::Point2f{x0, y0}, cv::Point2f{x1, y1}));
    return std::make_pair(point2f{from.x, from.y}, point2f{to.x, to.y});
}

Mat CvUtil::Resize(const Mat &mat, const Size<int> &dstSize) {
    auto matPtr = mat.getHolder();
    assert(matPtr);
    const auto&[dstW, dstH]=dstSize;
    Mat result(mat.getChannel(), mat.getDataType(), dstW, dstH);
    auto resPtr = result.getHolder();
    assert(resPtr);

    cv::resize(*matPtr, *resPtr, cv::Size{dstW, dstH}, 0, 0, cv::INTER_CUBIC);
    result.sync();

    return result;
}

std::pair<Mat, point3f> CvUtil::PadTo(
        const Mat &mat, const Size<int> &dstSize, const rgb &color) {
    const auto &src = *(mat.getHolder());
    Mat result(mat.getChannel(), mat.getDataType(), mat.getWidth(), mat.getHeight());
    auto &dst = *(result.getHolder());

    const auto&[dstW, dstH]=dstSize;
    int w = mat.getWidth();
    int h = mat.getHeight();
    int dw = std::max(0, dstW - w), dh = std::max(0, dstH - h);
    cv::copyMakeBorder(
            src, dst,
            dh / 2, dh - dh / 2, dw / 2, dw - dw / 2,
            cv::BORDER_CONSTANT, convertToScalar(color));
//    std::cout<<dw<<","<<dh<<","<<ret.cols<<","<<ret.rows<<std::endl;
    result.sync();
    return {std::move(result), {1.0f, dw / 2.f, dh / 2.f}};
}

std::pair<Mat, point3f> CvUtil::ResizeKeepRatio(const Mat &mat, const Size<int> &dstSize, const ColorName &color) {
    const auto &src = *(mat.getHolder());
    Mat result(mat.getChannel(), mat.getDataType(), mat.getWidth(), mat.getHeight());
    auto &dst = *(result.getHolder());

    const auto&[dstW, dstH]=dstSize;
    int w = mat.getWidth();
    int h = mat.getHeight();

    float kw = (float) dstW / w, kh = (float) dstH / h;
    float k = std::min(kw, kh);
    int newWidth = k * w, newHeight = k * h;
//    std::cout << w << "," << h << "," << k << "," << std::endl;
    cv::resize(src, dst, cv::Size(newWidth, newHeight),
               0, 0, cv::INTER_CUBIC);
    int dw = std::max(0, dstW - newWidth), dh = std::max(0, dstH - newHeight);
    cv::copyMakeBorder(
            dst, dst,
            dh / 2, dh - dh / 2, dw / 2, dw - dw / 2,
            cv::BORDER_CONSTANT, convertToScalar(color));
//    std::cout<<dw<<","<<dh<<","<<ret.cols<<","<<ret.rows<<std::endl;
    result.sync();
    return {std::move(result), {k, dw / 2.f, dh / 2.f}};
}


Mat CvUtil::ResizeWithBlock(const Mat &mat, const Size<int> &dstSize, const Size<int> &dstBlock) {
    auto matPtr = mat.getHolder();
    assert(matPtr);
    const auto&[dstW, dstH]=dstSize;
    Mat result(mat.getChannel(), mat.getDataType(), dstW, dstH);
    auto resPtr = result.getHolder();
    assert(resPtr);
    cv::resize(*matPtr, *resPtr, cv::Size{dstW, dstH}, 0, 0, cv::INTER_CUBIC);
    auto &procImg = *resPtr;
    auto[bW, bH]=dstBlock;
    bW -= dstW % bW;
    bH -= dstH % bH;
    bW /= 2;
    bH /= 2;
    // TODO: check if vconcat work as expected
    const auto &rgb = ColorUtil::GetRGB(ColorName::rgbWhite);
    cv::copyMakeBorder(procImg, procImg, bW, bW, bH, bH, cv::BORDER_CONSTANT, convertToScalar(rgb));
//    cv::vconcat(procImg, cv::Mat(bW, procImg.cols, procImg.type(), color), procImg);
//    cv::vconcat(cv::Mat(bW, procImg.cols, procImg.type(), color), procImg, procImg);
//    cv::hconcat(procImg, cv::Mat(procImg.rows, bH, procImg.type(), color), procImg);
//    cv::hconcat(cv::Mat(procImg.rows, bH, procImg.type(), color), procImg, procImg);
//    cv::imshow("", procImg);
//    cv::waitKey(0);
    result.sync();
    return result;
}

float CvUtil::GetInterArea(const rectf &r0, const rectf &r1) {
    const auto&[p0, p1]=r0;
    const auto&[p2, p3]=r1;
    cv::Rect2f rect1(cv::Point2f{p0.first, p0.second}, cv::Point2f{p1.first, p1.second});
    cv::Rect2f rect2(cv::Point2f{p2.first, p2.second}, cv::Point2f{p3.first, p3.second});
    cv::Rect_<float> inter = rect1 & rect2;
    return inter.area();
}

Mat CvUtil::Rotate(const Mat &mat, const float &angle) {
    auto matPtr = mat.getHolder();
    assert(matPtr);
    const auto &srcImage = *matPtr;
    cv::Point2f center{srcImage.cols / 2.f, srcImage.rows / 2.f};
    cv::Mat M = cv::getRotationMatrix2D(center, angle, 1);
    Mat result(mat.getChannel(), mat.getDataType(), srcImage.cols, srcImage.rows);
    auto dstPtr = result.getHolder();
    assert(dstPtr);
    const auto &destImage = *dstPtr;
    cv::warpAffine(srcImage, destImage, M, cv::Size(srcImage.cols, srcImage.rows),
                   cv::INTER_CUBIC, cv::BORDER_CONSTANT,
                   cv::Scalar(255, 255, 255));
//    cv::erode(destImage, destImage, cv::Mat());
//    cv::dilate(destImage, destImage, cv::Mat());
    result.sync();
    return result;
}

std::pair<cv::Point2f, cv::Point2f> getLineFromTo(const cv::Mat &mat, const cv::Rect2f &box) {
    cv::Point2f from, to;
    float w = box.width, h = box.height, x = box.x, y = box.y;
    int xmin = (std::max)(0, (int) std::floor(x));
    int ymin = (std::max)(0, (int) std::floor(y));
    int xmax = (std::min)(mat.cols - 1, (int) std::ceil(x + w));
    int ymax = (std::min)(mat.rows - 1, (int) std::ceil(y + h));
    int iw = xmax - xmin + 1, ih = ymax - ymin + 1;
    // 边长足够小 or 足够窄，误差忽略不计，直接取中点作为起始点
    if (iw < sMinSize || ih < sMinSize || w / h > sLineThresh || h / w > sLineThresh) {
        if (w < h) {
            from.x = to.x = x + w / 2;
            from.y = y;
            to.y = y + h;
        } else {
            from.x = x;
            to.x = x + w;
            from.y = to.y = y + h / 2;
        }
        return {from, to};
    }
    const int roiSize = 32;
    cv::Mat roiArea;
    cv::resize(mat(cv::Rect2i(xmin, ymin, iw, ih)),
               roiArea, cv::Size(roiSize, roiSize));
    const int step = roiSize / 4;
    const int step2 = 2 * step;
    const int step3 = 3 * step;
    auto get_bw_tl_br = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(0, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step, step, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step3, step3, step, step)))
                + cv::sum(roiArea(cv::Rect2i(0, 0, step2, step2)))
                + cv::sum(roiArea(cv::Rect2i(step2, step2, step2, step2))));
    };
    auto get_bw_bl_tr = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step3, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(0, step3, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, 0, step2, step2)))
                + cv::sum(roiArea(cv::Rect2i(0, step2, step2, step2))));
    };
//    std::cout << get_bw_tl_br() << std::endl;
//    std::cout << get_bw_bl_tr() << std::endl;
//    cv::imshow("origin", mat(cv::Rect2i(xmin, ymin, iw, ih)));
//    cv::imshow("roi64", roiArea);
//    cv::waitKey(0);
    if (get_bw_tl_br() < get_bw_bl_tr()) {
        from = box.tl();
        to = box.br();
    } else {
        from.x = box.x;
        to.y = box.y;
        from.y = box.y + box.height;
        to.x = box.x + box.width;
    }
    return {from, to};
}

std::pair<cv::Point2f, cv::Point2f> getWedgeFromTo(const cv::Mat &mat, const cv::Rect2f &box) {
    cv::Point2f from, to;
    float w = box.width, h = box.height, x = box.x, y = box.y;
    int xmin = (std::max)(0, (int) std::floor(x));
    int ymin = (std::max)(0, (int) std::floor(y));
    int xmax = (std::min)(mat.cols - 1, (int) std::ceil(x + w));
    int ymax = (std::min)(mat.rows - 1, (int) std::ceil(y + h));
    int iw = xmax - xmin + 1, ih = ymax - ymin + 1;
    // 边长足够小 or 足够窄，误差忽略不计，直接取中点作为起始点
    if (iw < sMinSize || ih < sMinSize || w / h > sLineThresh || h / w > sLineThresh) {
        if (w < h) {
            from.x = to.x = x + w / 2;
            from.y = y;
            to.y = y + h;
            auto s1 = getScalarSum(cv::sum(mat(cv::Rect2i(
                    xmin, ymin, iw, ih / 2))));
            auto s2 = getScalarSum(cv::sum(mat(cv::Rect2i(
                    xmin, ymin + ih / 2, iw, ih / 2))));
            if (s1 < s2) {
                std::swap(from, to);
            }
        } else {
            from.x = x;
            to.x = x + w;
            from.y = to.y = y + h / 2;
            auto s1 = getScalarSum(cv::sum(mat(cv::Rect2i(
                    xmin, ymin, iw / 2, ih))));
            auto s2 = getScalarSum(cv::sum(mat(cv::Rect2i(
                    xmin + ih / 2, ymin, iw / 2, ih))));
            if (s1 < s2) {
                std::swap(from, to);
            }
        }
        return {from, to};
    }
    const int roiSize = 32;
    cv::Mat roiArea;
    cv::resize(mat(cv::Rect2i(xmin, ymin, iw, ih)),
               roiArea, cv::Size(roiSize, roiSize));
    const int step = roiSize / 4;
    const int step2 = 2 * step;
    const int step3 = 3 * step;
    auto get_bw_tl = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(0, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step, step, step, step)))
                + cv::sum(roiArea(cv::Rect2i(0, 0, step2, step2))));
    };
    auto get_bw_br = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step2, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step3, step3, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step2, step2, step2))));
    };
    auto get_bw_bl = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(0, step3, step, step)))
                + cv::sum(roiArea(cv::Rect2i(0, step2, step2, step2))));
    };
    auto get_bw_tr = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step3, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, 0, step2, step2))));
    };
//    std::cout << get_bw_tl_br() << std::endl;
//    std::cout << get_bw_bl_tr() << std::endl;
//    cv::imshow("origin", mat(cv::Rect2i(xmin, ymin, iw, ih)));
//    cv::imshow("roi64", roiArea);
//    cv::waitKey(0);
    auto s1 = get_bw_tl(), s2 = get_bw_tr(), s3 = get_bw_bl(), s4 = get_bw_br();
    if (s1 + s4 < s2 + s3) {
        from = box.tl();
        to = box.br();
        if (s1 < s4) {
            std::swap(from, to);
        }
    } else {
        from.x = box.x;
        to.y = box.y;
        from.y = box.y + box.height;
        to.x = box.x + box.width;
        if (s3 < s2) {
            std::swap(from, to);
        }
    }
//    std::cout<<s1<<","<<s2<<"\n"<<s3<<","<<s4<<"\n-------"<<std::endl;
//    cv::imshow("roi64", roiArea);
//    cv::Mat displayImg;
//    cv::cvtColor(mat,displayImg,cv::COLOR_GRAY2BGR);
//    cv::rectangle(displayImg, rect, getScalar(ColorName::rgbGreen), 2);
//    cv::circle(displayImg, from, 4, getScalar(ColorName::rgbBlue), -1);
//    cv::circle(displayImg, to, 4, getScalar(ColorName::rgbRed), -1);
//    cv::imshow("bond", displayImg);
//    cv::waitKey(0);
    return {from, to};
}

/**
 * 在单通道黑白[0-255]位图上，寻找非 _bgPixel 像素的最小正包围盒
 * @param _uMat 填充为uchar，与 _bgPixel 可作相等比较
 * @param _bgPixel 背景颜色值
 * @return 正包围盒
 */
static std::optional<cv::Rect2i> getBoundBoxForBWFont(const cv::Mat &_uMat, const uchar &_bgPixel = 255) {
    int xmin;
    for (int x = 0; x < _uMat.cols; x++) {
        for (int y = 0; y < _uMat.rows; y++) {
            if (_bgPixel != _uMat.at<uchar>(y, x)) {
                xmin = x;
                goto L1;
            }
        }
    }
    return std::nullopt;
    L1:;
    int xmax;
    for (int x = _uMat.cols - 1; x >= xmin; x--) {
        for (int y = _uMat.rows - 1; y >= 0; y--) {
            if (_bgPixel != _uMat.at<uchar>(y, x)) {
                xmax = x;
                goto L2;
            }
        }
    }
    xmax = xmin;
    L2:;
    int ymin;
    for (int y = 0; y < _uMat.rows; y++) {
        for (int x = xmin; x <= xmax; x++) {
            if (_bgPixel != _uMat.at<uchar>(y, x)) {
                ymin = y;
                goto L3;
            }
        }
    }
    ymin = _uMat.rows - 1;
    L3:;
    int ymax;
    for (int y = _uMat.rows - 1; y >= ymin; y--) {
        for (int x = xmin; x <= xmax; x++) {
            if (_bgPixel != _uMat.at<uchar>(y, x)) {
                ymax = y;
                goto L4;
            }
        }
    }
    ymax = ymin;
    L4:;
    return cv::Rect2i(xmin, ymin, xmax - xmin + 1, ymax - ymin + 1);
}

Mat CvUtil::GetFont(const std::string &_text, const std::string &_fontFamily) {
//    return cv::Mat(32,32,CV_8UC1,cv::Scalar(0));
    QString qData;
    for (const auto &c: _text) {
        if ('0' <= c && c <= '9') {
            qData.push_back(QString("<sub>") + c + "</sub>");
        } else if ('#' == c) {
            qData.append("≡");
        } else if ('+' == c) {
            if (StdUtil::byProb(0.5))
                qData.append(QString("<sup>+</sup>"));
            else
                qData.append(QString("<sup>⊕</sup>"));
        } else if ('-' == c) {
            if (StdUtil::byProb(0.5))
                qData.append(QString("<sup>-</sup>"));
            else
                qData.append(QString("<sup>㊀</sup>"));
        } else {
            qData.append(c);
        }
    }
    QFont font;
    font.setFamily(_fontFamily.c_str());
    font.setWeight(1);
    font.setItalic(StdUtil::byProb(0.5));

    QImage image(1280, 164, QImage::Format_Grayscale8);
    image.fill(Qt::white);
    QPainter painter(&image);
    painter.setFont(font);
    QTextDocument td;
    td.setDefaultFont(font);
    td.setHtml(qData);
    float k = 5;
    painter.scale(k, k);
    painter.translate(0, 0);
    // FIXME: why QTextDocument.drawContents cant run as parallel job through openmp?
    td.drawContents(&painter);
    cv::Mat cvImg(image.height(), image.width(),
                  CV_8UC1, (void *) image.constBits(), image.bytesPerLine());
    auto rectPtr = getBoundBoxForBWFont(cvImg);
    if (rectPtr) {
        cvImg = cvImg(cv::Rect(rectPtr.value()));
    }
//    cv::imshow("1",cvImg);
//    cv::waitKey(0);
    Mat mat(MatChannel::GRAY, DataType::UINT8, image.width(), image.height());
    *mat.getHolder() = cvImg.clone();
    return mat;
}


/**
 *  版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
    本文链接：https://blog.csdn.net/iracer/article/details/49383491
 */
static void salt_pepper(cv::Mat image, int n) {
    int i, j;
    static std::vector<int> noise{0, 255};
    for (int k = 0; k < n / 2; k++) {
        // rand() is the random number generator
        i = StdUtil::randInt() % image.cols; // % 整除取余数运算符,rand=1022,cols=1000,rand%cols=22
        j = StdUtil::randInt() % image.rows;
        if (image.type() == CV_8UC1) { // gray-level image
            image.at<uchar>(j, i) = StdUtil::randSelect(noise); //at方法需要指定Mat变量返回值类型,如uchar等
        } else if (image.type() == CV_8UC3) { // color image
            image.at<cv::Vec3b>(j, i)[0] = StdUtil::randSelect(noise); //cv::Vec3b为opencv定义的一个3个值的向量类型
            image.at<cv::Vec3b>(j, i)[1] = StdUtil::randSelect(noise); //[]指定通道，B:0，G:1，R:2
            image.at<cv::Vec3b>(j, i)[2] = StdUtil::randSelect(noise);
        }
    }
}

static QImage binaryAlphaImage(cv::Mat &src) {
    cv::Mat result;
    cv::cvtColor(src, result, cv::COLOR_RGBA2GRAY);
    cv::adaptiveThreshold(result, result, 255.0,
                          cv::ADAPTIVE_THRESH_GAUSSIAN_C,
                          cv::THRESH_BINARY, 21, 10);
    cv::cvtColor(result, result, cv::COLOR_GRAY2BGRA);
    const uchar *pSrc = (const uchar *) result.data;
    // Create QImage with same dimensions as input Mat
    QImage image(pSrc, result.cols, result.rows, result.step, QImage::Format_ARGB32);
    return image.copy();
}

static cv::Mat convertQImageToMat(const QImage &_img) {
    cv::Mat mat;
    if (_img.isNull()) {
        std::cerr << "you are converting an empty QImage to cv::Mat" << std::endl;
        exit(-1);
    }
    void *dataPtr = const_cast<uchar *>(_img.constBits());
    int step = _img.bytesPerLine();
    switch (_img.format()) {
        case QImage::Format_RGB32:
        case QImage::Format_ARGB32:
        case QImage::Format_ARGB32_Premultiplied:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC4, dataPtr, step);
            break;
        case QImage::Format_RGB888:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC3, dataPtr, step);
            cv::cvtColor(mat, mat, cv::COLOR_BGR2RGB);
            break;
        case QImage::Format_Indexed8:
            mat = cv::Mat(_img.height(), _img.width(), CV_8UC1, dataPtr, step);
            break;
        default: {
            std::cerr << "unImpl image format:" << _img.format() << std::endl;
            exit(-1);
        }
    }
//    std::cout << mat.cols << "x" << mat.rows << std::endl;
    return std::move(mat);
}

static cv::Mat convertQPixmapToMat(const QPixmap &_img) {
    return convertQImageToMat(_img.toImage());
}


Mat CvUtil::AddGaussianNoise(const Mat &mat) {
    const auto &src = *(mat.getHolder());
    Mat result(mat.getChannel(), mat.getDataType(), mat.getWidth(), mat.getHeight());
    auto &dst = *(result.getHolder());

    cv::Mat noise(src.size(), CV_32F);
    cv::randn(noise, 0, StdUtil::belowProb(0.1));
    cv::Mat tmp;
    src.convertTo(tmp, CV_32F);
    src /= 255;
    tmp += noise;
    cv::normalize(tmp, tmp, 1.0, 0, cv::NORM_MINMAX, CV_32F);
    tmp *= 255;
    tmp.convertTo(dst, CV_8U);
    return result;
}

Mat CvUtil::AddSaltPepperNoise(const Mat &mat, const int &n) {
    const auto &src = *(mat.getHolder());
    Mat result(mat.getChannel(), mat.getDataType(), mat.getWidth(), mat.getHeight());
    auto &dst = *(result.getHolder());
    dst = src;

    salt_pepper(dst, n);
    return result;
}

Mat CvUtil::RevertColor(const Mat &mat) {
    const auto &src = *(mat.getHolder());
    Mat result(mat.getChannel(), mat.getDataType(), mat.getWidth(), mat.getHeight());
    auto &dst = *(result.getHolder());

    cv::bitwise_not(src, dst);
    return result;
}
#if not defined(Q_OS_WASM)
Mat CvUtil::BufferToGrayMat(std::vector<unsigned char> &buffer) {
    cv::Mat mat = cv::imdecode(buffer, cv::IMREAD_GRAYSCALE);
    Mat result(MatChannel::GRAY, DataType::UINT8, mat.cols, mat.rows);
    auto &dst = *(result.getHolder());
    dst = mat;
    result.sync();
    return result;
}
#endif
Mat CvUtil::HConcat(const Mat &m1, const Mat &m2) {
    Mat result(m1.getChannel(), m1.getDataType(), 0, 0);
    auto &dst = *(result.getHolder());
    cv::hconcat(*(m1.getHolder()), *(m2.getHolder()), dst);
    result.sync();
    return result;
}
