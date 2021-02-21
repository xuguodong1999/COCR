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
#include "timer.hpp"
#include "text_recognition.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>

#ifdef Q_OS_LINUX
std::string ROOT_DIR = "/home/xgd/source/repos/jokejoker/workspace/";
#elif defined(Q_OS_WIN)
std::string ROOT_DIR = "C:/Users/xgd/Desktop/jokejoker/workspace/";
#endif

std::string std_alphabet = "-=#+_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabdefghijnqrty";

void test_crnn_opencv() {
    xgd::TextRecognitionOpenCVSolver solver;
    solver.initModel(ROOT_DIR + "crnn_sim_3200.onnx", std_alphabet, 3200);
    cv::Mat image = cv::imread(ROOT_DIR + "demo.jpg", cv::IMREAD_GRAYSCALE);
    auto[strRes, scores]=solver.recognize(image);
    cv::imshow("result=" + strRes, image);
    cv::waitKey(0);
}

void test_crnn_ncnn() {
    xgd::TextRecognitionNCNNSolver solver;
    if (!solver.initModel(ROOT_DIR + "../resources/model/vgg_lstm_57_fp16.bin",
                          ROOT_DIR + "../resources/model/vgg_lstm_57_fp16.param",
                          std_alphabet, 3200)) {
        std::cerr << "fail to load crnn from ncnn" << std::endl;
    }
    while (true) {
        std::cout<<ROOT_DIR + "demo.jpg"<<std::endl;
        cv::Mat image = cv::imread(ROOT_DIR + "demo.jpg", cv::IMREAD_GRAYSCALE);
        Timer timer;
        timer.start();
        auto[strRes, scores]=solver.recognize(image);
        timer.start();
//    for (size_t i = 0; i < strRes.length(); i++) {
//        std::cout << strRes[i] << ":" << scores[i] << std::endl;
//    }
        std::cout << "result=" << strRes << std::endl;
        cv::destroyAllWindows();
        cv::imshow("result=" + strRes, image);
        cv::waitKey(0);
    }
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
