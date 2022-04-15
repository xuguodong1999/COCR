#include "cocr/text_recognizer.h"
#include "cocr/text_corrector.h"
#include "ocv/algorithm.h"
#ifdef USE_OPENCV_DNN

#include "text_recognizer_opencv_impl.h"

#else

#include "text_recognizer_ncnn_impl.h"

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
    std::string onnxTextModel = MODEL_DIR + std::string("/deprecated/onnx-crnn-57.onnx");
    auto recognizer = std::make_shared<TextRecognizerOpenCVImpl>();
    if (!recognizer->initModel(onnxTextModel, TextCorrector::GetAlphabet(), 192)) {
        qDebug() << "fail to init opencv recognizer";
        recognizer->freeModel();
        return nullptr;
    }
#else
    std::string ncnnTextModel = MODEL_DIR + std::string("/crnn57.fp16.bin");
    std::string ncnnTextModelCfg = MODEL_DIR + std::string("/crnn57.fp16.param");
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


