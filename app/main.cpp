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
    xgd::TextRecognitionOpenCVImpl solver;
    solver.initModel(ROOT_DIR + "crnn_sim_3200.onnx", std_alphabet, 3200);
    cv::Mat image = cv::imread(ROOT_DIR + "demo.jpg", cv::IMREAD_GRAYSCALE);
    auto[strRes, scores]=solver.recognize(image);
    cv::imshow("result=" + strRes, image);
    cv::waitKey(0);
}

void test_crnn_ncnn() {
    xgd::TextRecognitionNcnnImpl solver;
    if (!solver.initModel(ROOT_DIR + "../resources/model/vgg_lstm_57_fp16.bin",
                          ROOT_DIR + "../resources/model/vgg_lstm_57_fp16.param",
                          std_alphabet, 3200)) {
        std::cerr << "fail to load crnn from ncnn" << std::endl;
    }
    while (true) {
        std::cout << ROOT_DIR + "demo.jpg" << std::endl;
        cv::Mat image = cv::imread(ROOT_DIR + "demo.jpg", cv::IMREAD_GRAYSCALE);
//        Timer timer;
//        timer.start();
        auto[strRes, scores]=solver.recognize(image);
//        timer.start();
//    for (size_t i = 0; i < strRes.length(); i++) {
//        std::cout << strRes[i] << ":" << scores[i] << std::endl;
//    }
        std::cout << "result=" << strRes << std::endl;
        cv::destroyAllWindows();
        cv::imshow("result=" + strRes, image);
        cv::waitKey(0);
    }
}

#include "object_detection.hpp"
#include <opencv2/imgproc.hpp>

//void runModel(xgd::ObjectDetector &_detector, xgd::TextRecognition &_recognizer, cv::Mat &_originInput) {
//    auto[input, result] = _detector.detect(_originInput);
//
//}
//
//void displayResult(const cv::Mat &_input, const std::vector<xgd::DetectorObject> &_objects) {
//    xgd::TextRecognitionNcnnImpl solver;
//    if (!solver.initModel(ROOT_DIR + "../resources/model/vgg_lstm_57_fp16.bin",
//                          ROOT_DIR + "../resources/model/vgg_lstm_57_fp16.param",
//                          std_alphabet, 3200)) {
//        std::cerr << "fail to load crnn from ncnn" << std::endl;
//    }
//    cv::Mat canvas;
//    cv::cvtColor(_input, canvas, cv::COLOR_GRAY2BGR);
////    auto colorIdx = [](const int &_a) -> ColorName {
////        return static_cast<const ColorName>((7 + _a) * 5123 % 455);
////    };
//    static std::vector<std::string> names = {
//            "-", "=", "#", "-+", "--", "~", "o", "s"
//    };
//    auto round_scale = [&](const float &_x, const float &_y, const float &_w, const float &_h) -> cv::Rect {
//        int x, y, w, h;
//        int s = 4;
//        x = std::max(0, (int) std::round(_x - s));
//        y = std::max(0, (int) std::round(_y - s));
//        w = std::min(_input.cols - 1, (int) std::round(_x + _w + 2 * s)) - x;
//        h = std::min(_input.rows - 1, (int) std::round(_y + _h + 2 * s)) - y;
//        return cv::Rect(x, y, w, h);
//    };
//    for (auto &obj:_objects) {
//        size_t idx = std::min((size_t) obj.label, names.size());
//        std::string info;
//        if (idx == 7) {
//            auto[strRes, scores]=solver.recognize(_input(round_scale(obj.x, obj.y, obj.w, obj.h)));
//            info = strRes;
//        } else {
//            info = names[idx];
//        }
////        info += "," + to_string_with_precision(obj.prob, 2);
////        cv::putText(canvas, info, cv::Point(obj.x, obj.y),
////                    1, 1.2, getScalar(colorIdx(obj.label)),
////                    2, cv::LINE_AA, false);
////        cv::rectangle(canvas, cv::Rect(obj.x, obj.y, obj.w, obj.h),
////                      getScalar(colorIdx(idx)), 1);
//    }
//    cv::destroyAllWindows();
//    cv::imshow("", canvas);
//    cv::waitKey(0);
//}
#include "ocr_manager.hpp"

void testYolo() {
    xgd::ObjectDetectorOpenCVImpl detector;
    if (!detector.initModel(
            "/home/xgd/datasets/soso-obj/yolov4-small-3l/soso-obj.cfg",
            "/home/xgd/datasets/soso-obj/yolov4-small-3l/weights/yolov4-smallest-3l_last.weights")) {
        std::cerr << "fail to load yolo from opencv" << std::endl;
    }
    detector.setConfThresh(0.25);
    detector.setIouThresh(0.45);
    xgd::TextRecognitionNcnnImpl solver;
    if (!solver.initModel(ROOT_DIR + "../resources/model/vgg_lstm_57_fp16.bin",
                          ROOT_DIR + "../resources/model/vgg_lstm_57_fp16.param",
                          std_alphabet, 3200)) {
        std::cerr << "fail to load crnn from ncnn" << std::endl;
    }
    cv::Mat image = cv::imread("/media/xgd/hjyy-ext4/soso17_small/JPEGImages/2_0.jpg",
                               cv::IMREAD_GRAYSCALE);
    xgd::GraphComposer composer;
    xgd::OCRManager ocrManager(detector, solver, composer);
    ocrManager.ocr(image, true);
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
