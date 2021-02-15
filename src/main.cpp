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

#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <fstream>

std::string ROOT_DIR="/home/xgd/source/repos/jokejoker/";
std::string std_alphabet = "-=#+_()0123456789ABDEFGHLNQRTYabcdefghijklmnopqrstuvwxyz";
void test_crnn() {
    int rgb = cv::IMREAD_GRAYSCALE;
    cv::Mat image = cv::imread(
    ROOT_DIR+"soso-data/demo.jpg",
//            "/home/xgd/source/crnn.pytorch/data/demo2.png",
            rgb);
    cv::dnn::TextRecognitionModel model(
            ROOT_DIR+"crnn.pytorch/crnn.onnx"//1x47x57 47-192
//            "/home/xgd/source/crnn.pytorch/data/crnn.onnx"//26x1x37 26-100
            );
    auto &net = model.getNetwork_();
//    for (auto &l:net.getLayerNames()) {
//        std::cout << l << std::endl;
//    }
    model.setDecodeType("CTC-greedy");
    std::vector<std::string> vocabulary;
    for(auto&s:std_alphabet){
        vocabulary.push_back(std::string(1,s));
    }

    model.setVocabulary(vocabulary);
    auto vb=model.getVocabulary();
    for(auto&v:vb){
        std::cout<<v<<std::endl;
    }
    double scale = 1.0 / 127.5;
    cv::Scalar mean = cv::Scalar(127.5, 127.5, 127.5);

    cv::Size inputSize = cv::Size(192, 32);
    model.setInputParams(scale, inputSize, mean);

    std::string recognitionResult = model.recognize(image);
    std::cout << "'" << recognitionResult << "'" << std::endl;
    cv::imshow("result=" + recognitionResult, image);
    cv::waitKey(0);
}

int main(int argc, char **args) {
    try {
        test_crnn();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return 0;
}
