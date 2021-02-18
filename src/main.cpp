//#include <QApplication>
//#include <QWidget>
//
//int main(int argc, char *argv[]) {
//#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif
//    QApplication app(argc, argv);
//    (new QWidget)->show();
//    return app.exec();
//}

#include "text_recognition.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>

std::string ROOT_DIR = "/home/xgd/source/repos/jokejoker/workspace/";
std::string std_alphabet = "-=#+_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabdefghijnqrty";

void test_crnn_opencv() {
    xgd::TextRecognitionOpenCVSolver solver;
    solver.initModel(ROOT_DIR + "crnn_sim_3200.onnx", std_alphabet, 3200);
    cv::Mat image = cv::imread(ROOT_DIR + "demo.jpg", cv::IMREAD_GRAYSCALE);
    cv::hconcat(image, image, image);
    auto[strRes, scores]=solver.recognize(image);
    cv::imshow("result=" + strRes, image);
    cv::waitKey(0);
}

void test_crnn_ncnn() {
    xgd::TextRecognitionNCNNSolver solver;
    if (!solver.initModel(ROOT_DIR + "crnn_3200.bin",
                          ROOT_DIR + "crnn_3200.param",
                          std_alphabet)) {
        std::cerr << "fail to load crnn from ncnn" << std::endl;
    }
    cv::Mat image = cv::imread(ROOT_DIR + "demo.jpg", cv::IMREAD_GRAYSCALE);
    auto[strRes, scores]=solver.recognize(image);
    for (size_t i = 0; i < strRes.length(); i++) {
        std::cout << strRes[i] << ":" << scores[i] << std::endl;
    }
    cv::imshow("result=" + strRes, image);
    cv::waitKey(0);
}

int main(int argc, char **args) {
    try {
        test_crnn_ncnn();
//        test_crnn_opencv();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
