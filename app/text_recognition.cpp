#include "text_recognition.hpp"
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <numeric>
#include <iostream>

bool xgd::TextRecognitionNCNNSolver::initModel(
        const std::string &_ncnnBin, const std::string &_ncnnParam, const std::string &_words) {
    int ret_param = net.load_param(_ncnnParam.c_str());
    int ret_bin = net.load_model(_ncnnBin.c_str());
    if (ret_param != 0 || ret_bin != 0) {
        return false;
    }
    for (auto &c:_words) {
        wordVec.push_back(std::string(1, c));
    }
    return true;
}

std::pair<std::string, std::vector<float>> xgd::TextRecognitionNCNNSolver::recognize(
        cv::Mat &_originImage) {
    float scaleX = (float) dstHeight / (float) _originImage.rows;
    int dstWidth = int((float) _originImage.cols * scaleX);

    cv::Mat srcResize;
    cv::resize(_originImage, srcResize, cv::Size(dstWidth, dstHeight));
    ncnn::Mat input = ncnn::Mat::from_pixels(
            srcResize.data, ncnn::Mat::PIXEL_GRAY,
            srcResize.cols, srcResize.rows);

    const float mv[3] = {meanValues, meanValues, meanValues}, nv[3] = {normValues, normValues, normValues};
    input.substract_mean_normalize(mv, nv);

    ncnn::Extractor extractor = net.create_extractor();
    extractor.set_num_threads(numThread);
    extractor.input("input", input);
    ncnn::Mat out;
    extractor.extract("out", out);
    return recognize((float *) out.data, out.h, out.w);
}

void xgd::TextRecognitionNCNNSolver::freeModel() {
    net.clear();
}

template<class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::max_element(first, last));
}

std::pair<std::string, std::vector<float>> xgd::TextRecognitionNCNNSolver::recognize(
        const float *_outputData, const int &_h, const int _w) {
    std::string strRes;
    std::vector<float> scores;
    int lastIndex = 0;
    for (int i = 0; i < _h; i++) {
        //do softmax
        std::vector<float> exps(_w);
        for (int j = 0; j < _w; j++) {
            exps[j] = std::exp(_outputData[i * _w + j]);
        }
        float partition = std::accumulate(exps.begin(), exps.end(), 0.0);//row sum
        int maxIndex = int(argmax(exps.begin(), exps.end()));
        float maxValue = float(*std::max_element(exps.begin(), exps.end())) / partition;
        if (maxIndex > 0 && maxIndex < wordVec.size() && (!(i > 0 && maxIndex == lastIndex))) {
            scores.emplace_back(maxValue);
            strRes.append(wordVec[maxIndex - 1]);
        }
        lastIndex = maxIndex;
    }
    return {strRes, scores};
}

xgd::TextRecognitionNCNNSolver::TextRecognitionNCNNSolver() :numThread(4){

}

bool xgd::TextRecognitionOpenCVSolver::initModel(const std::string &_onnxFile, const std::string &_words, int _width) {
    try {
        model = cv::dnn::TextRecognitionModel(_onnxFile);
        model.setDecodeType("CTC-greedy");
        for (auto &c:_words) {
            wordVec.push_back(std::string(1, c));
        }
        model.setVocabulary(wordVec);
        model.setInputParams(normValues, cv::Size(_width, dstHeight), cv::Scalar(meanValues));
    } catch (...) {
        return false;
    }
    return true;
}

void xgd::TextRecognitionOpenCVSolver::freeModel() {

}

std::pair<std::string, std::vector<float>> xgd::TextRecognitionOpenCVSolver::recognize(cv::Mat &_originImage) {
    std::string recognitionResult = model.recognize(_originImage);
    std::vector<float> scores(recognitionResult.size(), -1);
    return {recognitionResult, scores};
}

void xgd::TextRecognitionSolver::preProcess(cv::Mat &_src, const int &_dstWidth) {

}
