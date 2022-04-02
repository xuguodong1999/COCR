#include "ocr/object_detector.hpp"
#include "opencv_util/opencv_util.hpp"

#include <QDebug>

#ifdef USE_OPENCV_DNN

#include "../ocr_impl/object_detector_opencv_impl.hpp"

#else

#include "../ocr_impl/object_detector_ncnn_impl.hpp"

#endif

#include <opencv2/imgproc.hpp>

#include <iostream>

/**
 * 默认行为：转单通道，边长向上转 sizeBase 的倍数，边长限制到 [maxWidth,maxHeight]
 * @param _src
 * @return 可以送进网络的图像
 */
cv::Mat cocr::ObjectDetector::preProcess(const cv::Mat &_src) {
    if (_src.empty())throw std::runtime_error("get empty image in ObjectDetector::preProcess");
    int w = _src.cols, h = _src.rows;
    if (w > maxWidth || h > maxHeight) {
        float kw = static_cast<float >(maxWidth) / w, kh = static_cast<float >(maxHeight) / h;
        float k = (std::min)(kw, kh);
        w *= k;
        h *= k;
    }
    w += (sizeBase - w % sizeBase);
    h += (sizeBase - h % sizeBase);
    if (w > maxWidth) w -= sizeBase;
    if (h > maxHeight)h -= sizeBase;
    cv::Mat procImg;
    if (3 == _src.channels()) {
        cv::cvtColor(_src, procImg, cv::ColorConversionCodes::COLOR_BGR2GRAY);
        if (w != _src.cols || h != _src.rows) {
            cv::resize(procImg, procImg, cv::Size(w, h), 0, 0, cv::INTER_CUBIC);
        }
    } else if (4 == _src.channels()) {
        cv::cvtColor(_src, procImg, cv::ColorConversionCodes::COLOR_BGRA2GRAY);
        if (w != _src.cols || h != _src.rows) {
            cv::resize(procImg, procImg, cv::Size(w, h), 0, 0, cv::INTER_CUBIC);
        }
    } else {
        if (w != _src.cols || h != _src.rows) {
            cv::resize(_src, procImg, cv::Size(w, h), 0, 0, cv::INTER_CUBIC);
        } else {
            procImg = _src;
        }
    }
    const size_t baseSize_2 = sizeBase / 2;
    cv::vconcat(procImg, cv::Mat(baseSize_2, procImg.cols, procImg.type(), getScalar(ColorName::rgbWhite)), procImg);
    cv::vconcat(cv::Mat(baseSize_2, procImg.cols, procImg.type(), getScalar(ColorName::rgbWhite)), procImg, procImg);
    cv::hconcat(procImg, cv::Mat(procImg.rows, baseSize_2, procImg.type(), getScalar(ColorName::rgbWhite)), procImg);
    cv::hconcat(cv::Mat(procImg.rows, baseSize_2, procImg.type(), getScalar(ColorName::rgbWhite)), procImg, procImg);
    return procImg;
}

cocr::ObjectDetector::ObjectDetector() : maxHeight(1280), maxWidth(1280) {

}


std::shared_ptr<cocr::ObjectDetector> cocr::ObjectDetector::MakeInstance() {
#ifdef USE_OPENCV_DNN
    std::string ocvDetModel = MODEL_DIR + std::string("/deprecated/darknet-yolo-3l-c8.weights");
    std::string ocvDetModelCfg = MODEL_DIR + std::string("/deprecated/darknet-yolo-3l-c8.cfg");
    auto detector = std::make_shared<cocr::ObjectDetectorOpenCVImpl>();
    detector->setConfThresh(0.25);
    detector->setIouThresh(0.45);
    if (!detector->initModel(ocvDetModelCfg, ocvDetModel)) {
        qDebug() << "fail to init opencv detector";
        detector->freeModel();
        return nullptr;
    }
    qDebug() << "init opencv detector success";
#else
    std::string ncnnDetModel = MODEL_DIR + std::string("/det8.fp16.bin");
    std::string ncnnDetModelCfg = MODEL_DIR + std::string("/det8.fp16.param");
    auto detector = std::make_shared<cocr::ObjectDetectorNcnnImpl>();
    detector->setNumThread(4);
    if (!detector->initModel(ncnnDetModel, ncnnDetModelCfg, 1280)) {
        qDebug() << "fail to init ncnn detector";
        detector->freeModel();
        return nullptr;
    }
    qDebug() << "init ncnn detector success";
#endif
    return detector;
}

