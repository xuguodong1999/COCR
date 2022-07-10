#include "cocr/object_detector.h"
#include "ocv/algorithm.h"
#include <QDebug>

#ifdef USE_OPENCV_DNN

#include "object_detector_opencv_impl.h"

#else

#include "object_detector_ncnn_impl.h"

#endif

#include <iostream>

/**
 * 默认行为：转单通道，边长向上转 sizeBase 的倍数，边长限制到 [maxWidth,maxHeight]
 * @param _src
 * @return 可以送进网络的图像
 */
Mat ObjectDetector::preProcess(const Mat &_src) {
    int w = _src.getWidth(), h = _src.getHeight();
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
    return CvUtil::ResizeWithBlock(_src, {w, h}, {sizeBase, sizeBase});
}

ObjectDetector::ObjectDetector() : maxHeight(1280), maxWidth(1280) {

}


std::shared_ptr<ObjectDetector> ObjectDetector::MakeInstance() {
#ifdef USE_OPENCV_DNN
    std::string ocvDetModel = MODEL_DIR + std::string("/deprecated/darknet-yolo-3l-c8.weights");
    std::string ocvDetModelCfg = MODEL_DIR + std::string("/deprecated/darknet-yolo-3l-c8.cfg");
    auto detector = std::make_shared<ObjectDetectorOpenCVImpl>();
    detector->setConfThresh(0.25);
    detector->setIouThresh(0.45);
    if (!detector->initModel(ocvDetModelCfg, ocvDetModel)) {
        qDebug() << "fail to init opencv detector";
        detector->freeModel();
        return nullptr;
    }
    qDebug() << "init opencv detector success";
#else
    std::string ncnnDetModel = MODEL_DIR + std::string("/yolo_3l_c8.bin");
    std::string ncnnDetModelCfg = MODEL_DIR + std::string("/yolo_3l_c8.param");
    auto detector = std::make_shared<ObjectDetectorNcnnImpl>();
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

