#include "yolo.hpp"
#include "colors.hpp"
#include "std_util.hpp"
#include "timer.hpp"
#include <opencv2/imgproc.hpp>

#ifdef Q_OS_WIN64

#include <opencv2/highgui.hpp>

#endif

std::shared_ptr<YoloDetector>yoloDetector;

float YoloDetector::getIouThresh() const {
    return iouThresh;
}

void YoloDetector::setIouThresh(float iouThresh) {
    YoloDetector::iouThresh = iouThresh;
}

float YoloDetector::getConfThresh() const {
    return confThresh;
}

void YoloDetector::setConfThresh(float confThresh) {
    YoloDetector::confThresh = confThresh;
}

cv::Mat YoloDetector::detectAndDisplay(
        const cv::Mat &_img, const std::vector<std::string> &_names) {
    Timer timer;
    timer.start();
    auto objs = detect(_img);
    timer.stop(true);
    cv::Mat img = _img.clone();
    auto colorIdx = [](const int &_a) -> ColorName {
        return static_cast<const ColorName>((7 + _a) * 5123 % 455);
    };
    for (auto &obj:objs) {
        size_t idx = std::min((size_t) obj.label, _names.size());
        auto info = _names[idx] + "," + to_string_with_precision(obj.prob, 2);
        cv::putText(img, info, cv::Point(obj.x, obj.y),
                    1, 1.2, getScalar(colorIdx(obj.label)),
                    2, cv::LINE_AA, false);
        cv::rectangle(img, cv::Rect(obj.x, obj.y, obj.w, obj.h),
                      getScalar(colorIdx(idx)), 1);
    }
#ifdef Q_OS_WIN64
    cv::imshow("YoloForward", img);
//    cv::waitKey(0);
#endif
    return img;
}

