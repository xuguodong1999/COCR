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

cv::Mat rotateCvMat(const cv::Mat &srcImage, double angle) {
    cv::Mat destImage;
    cv::Point2f center(srcImage.cols / 2, srcImage.rows / 2);//中心
    cv::Mat M = cv::getRotationMatrix2D(center, angle, 1);//计算旋转的仿射变换矩阵
    cv::warpAffine(srcImage, destImage, M, cv::Size(srcImage.cols, srcImage.rows),
               cv::INTER_CUBIC,
               cv::BORDER_CONSTANT,
               cv::Scalar(255, 255, 255));//仿射变换
//    circle(destImage, center, 2, cv::Scalar(255,255,255));
    return destImage;
}
#include <QTime>
void testYolo() {
//    xgd::ObjectDetectorOpenCVImpl detector;
//    if (!detector.initModel(
//            ROOT_DIR + "../resources/model/yolo-3l-c8.cfg",
//            ROOT_DIR + "../resources/model/yolo-3l-c8.weights")) {
//        std::cerr << "fail to load yolo from opencv" << std::endl;
//    }
//    detector.setConfThresh(0.15);
//    detector.setIouThresh(0.45);

    xgd::ObjectDetectorNCNNImpl detector;
    detector.setNumThread(4);
    if (!detector.initModel(ROOT_DIR + "../resources/model/yolo_3l_c8.bin",
                            ROOT_DIR + "../resources/model/yolo_3l_c8.param",
                            1280)) {
        std::cerr << "fail to load yolo from ncnn" << std::endl;
    }

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
    QSet<size_t> badExample = {
            26, 35, 36, 38, 50
    };
    while (idx < fileList.size()) {
        auto &file = fileList[idx++];
//    while (true) {
//        auto &file = fileList[rand() % fileList.size()];
        if (file.suffix() != "TIF")continue;
//        if (!badExample.contains(idx))continue;
//        if(file.fileName()!="US07317008-20080108-C00027.TIF")continue;
        qDebug() << idx << ":" << file.fileName();
        cv::Mat image = cv::imread(file.absoluteFilePath().toStdString(), cv::IMREAD_GRAYSCALE);
//        cv::erode(image,image,cv::Mat());
//        image = rotateCvMat(image, 2);
//        cv::imshow("origin",image);
//        cv::waitKey(0);
        ocrManager.ocr(image, true);
    }
//    while (true) {
//        auto testImg = cv::imread(ROOT_DIR + "/demo.png", cv::IMREAD_GRAYSCALE);
////    cv::erode(testImg, testImg,cv::Mat());
//        ocrManager.ocr(testImg, true);
//    }
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
