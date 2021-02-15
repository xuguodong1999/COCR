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


void test_crnn() {
    int rgb = cv::IMREAD_GRAYSCALE;
    cv::Mat image = cv::imread(
            "/home/xgd/source/repos/jokejoker/soso-data/demo.jpg",
//            "/home/xgd/source/crnn.pytorch/data/demo2.png",
            rgb);
    cv::dnn::TextRecognitionModel model(
            "/home/xgd/source/ocr.pytorch/crnn.onnx"//1x47x57 47-192
//            "/home/xgd/source/crnn.pytorch/data/crnn.onnx"//26x1x37 26-100
            );
    auto &net = model.getNetwork_();
//    for (auto &l:net.getLayerNames()) {
//        std::cout << l << std::endl;
//    }
    model.setDecodeType("CTC-greedy");
    std::ifstream vocFile;
    vocFile.open(
            "/home/xgd/source/repos/jokejoker/soso-data/alphabet_56.txt"
//            "/home/xgd/source/crnn.pytorch/data/alphabet_36.txt"
            );
    CV_Assert(vocFile.is_open());
    cv::String vocLine;
    std::vector<cv::String> vocabulary;
    while (std::getline(vocFile, vocLine)) {
        vocabulary.push_back(vocLine);
    }
    std::cout << "vocabulary.size=" << vocabulary.size() << std::endl;
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
