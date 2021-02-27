#include <QApplication>
#include <QWidget>
//
//int main(int argc, char *argv[]) {
//#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif
//    QApplication app(argc, argv);
//    (new QWidget)->show();
//    return app.exec();
//}
#include <iostream>

#ifdef Q_OS_LINUX
std::string ROOT_DIR = "/home/xgd/source/repos/jokejoker/workspace/";
#elif defined(Q_OS_WIN)
std::string ROOT_DIR = "C:/Users/xgd/Desktop/jokejoker/workspace/";
#endif

#include "ocr_manager.hpp"
#include <opencv2/highgui.hpp>

#include <QDir>
#include <QDebug>
#include <opencv2/imgproc.hpp>

void testYolo() {
    xgd::ObjectDetectorOpenCVImpl detector;
    if (!detector.initModel(
            "/home/xgd/datasets/soso-obj/yolov4-small-3l/soso-obj.cfg",
            "/home/xgd/datasets/soso-obj/yolov4-small-3l/weights/yolov4-smallest-3l_last.weights")) {
        std::cerr << "fail to load yolo from opencv" << std::endl;
    }
    detector.setConfThresh(0.15);
    detector.setIouThresh(0.45);

    xgd::TextRecognitionNcnnImpl recognizer;
    if (!recognizer.initModel(ROOT_DIR + "../resources/model/vgg_lstm_57_fp16_mixFont.bin",
                              ROOT_DIR + "../resources/model/vgg_lstm_57_fp16.param",
                              xgd::TextCorrector::GetAlphabet(), 3200)) {
        std::cerr << "fail to load crnn from ncnn" << std::endl;
    }

    xgd::TextCorrector corrector;
    corrector.InitData();

    xgd::GraphComposer composer;

    xgd::OCRManager ocrManager(detector, recognizer, corrector, composer);
    QDir dir((ROOT_DIR + "/uspto-validation-updated/images").c_str());
    auto fileList = dir.entryInfoList(QDir::Filter(QDir::Files));
    srand(time(0));
    size_t idx = 0;
    while (true) {
        auto &file = fileList[rand() % fileList.size()];
//        auto &file = fileList[idx++];
        if (file.suffix() != "TIF")continue;
        qDebug() << file;
        cv::Mat image = cv::imread(file.absoluteFilePath().toStdString(), cv::IMREAD_GRAYSCALE);
//        cv::erode(image,image,cv::Mat());
        ocrManager.ocr(image, true);
    }
    while (true) {
        auto testImg = cv::imread(ROOT_DIR + "/demo.png", cv::IMREAD_GRAYSCALE);
//    cv::erode(testImg, testImg,cv::Mat());
        ocrManager.ocr(testImg, true);
    }
}

int main(int argc, char **args) {
    try {
        testYolo();
//        test_crnn_ncnn();
//        test_crnn_opencv();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
