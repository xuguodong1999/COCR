#include "XGD/COCR/TextRecognizer.h"
#include "XGD/COCR/TextCorrector.h"
#include "XGD/Vision/Algorithm.h"

#ifdef USE_OPENCV_DNN

#include "TextRecognizerOpenCVImpl.h"

#else

#include "TextRecognizerNCNNImpl.h"

#endif


#include <QDebug>

#include <iostream>

Mat TextRecognizer::preProcess(const Mat &_src) {
    // default behavior: resized to dstHeight
    float scaleX = (float) dstHeight / (float) _src.getHeight();
    int dstWidth = int((float) _src.getWidth() * scaleX);
    return CvUtil::Resize(_src, {dstWidth, dstHeight});
}

std::shared_ptr<TextRecognizer> TextRecognizer::MakeInstance() {
#ifdef USE_OPENCV_DNN
    std::string onnxTextModel = DEV_ASSETS_DIR + std::string("/models/onnx-crnn-c57/model.onnx");
    auto recognizer = std::make_shared<TextRecognizerOpenCVImpl>();
    if (!recognizer->initModel(onnxTextModel, TextCorrector::GetAlphabet(), 192)) {
        qDebug() << "fail to init opencv recognizer";
        recognizer->freeModel();
        return nullptr;
    }
#else
    std::string ncnnTextModel = ":/models/ncnn-crnn-c57-fp16-v2/model.bin";
    std::string ncnnTextModelCfg = ":/models/ncnn-crnn-c57-fp16-v2/model.param";
    auto recognizer = std::make_shared<TextRecognizerNcnnImpl>();
    recognizer->setNumThread(4);
    if (!recognizer->initModel(
            ncnnTextModel, ncnnTextModelCfg,
            TextCorrector::GetAlphabet(), 3200)) {
        qDebug() << "fail to init ncnn recognizer";
        recognizer->freeModel();
        return nullptr;
    }
    qDebug() << "init ncnn recognizer success";
#endif
    return recognizer;
}


