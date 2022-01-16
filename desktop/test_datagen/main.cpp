#include "hw_str.hpp"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <fstream>
#include <opencv2/highgui.hpp>

extern std::string WORKSPACE;

void test_crnn() {
    // Load a cropped text line image
    // you can find cropped images for testing in "Images for Testing"
    int rgb = cv::IMREAD_GRAYSCALE; // This should be changed according to the model input requirement.
    cv::Mat image = cv::imread("/home/xgd/source/repos/jokejoker/soso-data/test/JPEGImages/115"
                               ".jpg", rgb);

    // Load models weights
    cv::dnn::TextRecognitionModel model(
            "/home/xgd/source/ocr.pytorch/crnn.onnx");
    auto&net=model.getNetwork_();
    for (auto &l:net.getLayerNames()) {
        std::cout << l << std::endl;
    }
    // The decoding method
    // more methods will be supported in future
    model.setDecodeType("CTC-greedy");
    // Load vocabulary
    // vocabulary should be changed according to the text recognition model
    std::ifstream vocFile;
    vocFile.open(WORKSPACE + "/alphabet_56.txt");
    CV_Assert(vocFile.is_open());
    cv::String vocLine;
    std::vector<cv::String> vocabulary;
    while (std::getline(vocFile, vocLine)) {
        vocabulary.push_back(vocLine);
    }
    std::cout<<"vocabulary.size="<<vocabulary.size()<<std::endl;
    model.setVocabulary(vocabulary);
    ///////////////////////////////////
    // Normalization parameters
//    double scale = 1.0 / 255.0;
//    cv::Scalar mean = cv::Scalar(0,0,0);
    double scale = 1.0 / 127.5;
    cv::Scalar mean = cv::Scalar(127.5, 127.5, 127.5);
    // The input shape
    cv::Size inputSize = cv::Size(192, 32);
    model.setInputParams(scale, inputSize, mean);
//    model.setInputSize(inputSize);
    ////////////////////////////////////////////
    std::string recognitionResult = model.recognize(image);
    std::cout << "'" << recognitionResult << "'" << std::endl;
    cv::imshow("result="+recognitionResult, image);
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