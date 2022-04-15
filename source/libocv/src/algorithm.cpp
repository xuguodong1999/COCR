#include <opencv2/imgproc.hpp>
#include "ocv/algorithm.h"
#include <opencv2/highgui.hpp>

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
    std::swap(*affine, matrix);
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
    const auto&[r, g, b]=ColorUtil::GetRGB(ColorName::rgbWhite);
    cv::copyMakeBorder(procImg, procImg, bW, bW, bH, bH, cv::BORDER_CONSTANT, (r + g + b) / 3.0);
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
