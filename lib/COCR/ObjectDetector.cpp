#include "XGD/COCR/ObjectDetector.h"
#include "XGD/Vision/Algorithm.h"

#include <QDebug>

#ifdef USE_OPENCV_DNN

#include "ObjectDetectorOpenCVImpl.h"

#else

#include "ObjectDetectorNCNNImpl.h"

#endif

#include <iostream>

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
    std::string ocvDetModel = DEV_ASSETS_DIR + std::string("/models/darknet-yolov3-h3-c8/model.weights");
    std::string ocvDetModelCfg = DEV_ASSETS_DIR + std::string("/models/darknet-yolov3-h3-c8/model.cfg");
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
    std::string ncnnDetModel = ":/models/ncnn-yolov3-h3-c8/model.bin";
    std::string ncnnDetModelCfg = ":/models/ncnn-yolov3-h3-c8/model.param";
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

