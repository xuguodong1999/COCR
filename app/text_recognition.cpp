#include "text_recognition.hpp"
#include <opencv2/dnn.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <ncnn/net.h>
#include <numeric>
#include <iostream>

bool xgd::TextRecognitionNcnnImpl::initModel(
        const std::string &_ncnnBin, const std::string &_ncnnParam, const std::string &_words, const int &_maxWidth) {
    net = std::make_shared<ncnn::Net>();
    int ret_param = net->load_param(_ncnnParam.c_str());
    int ret_bin = net->load_model(_ncnnBin.c_str());
    if (ret_param != 0 || ret_bin != 0) {
        return false;
    }
    for (auto &c:_words) {
        wordVec.push_back(std::string(1, c));
    }
    maxWidth = _maxWidth;
    return true;
}

std::pair<std::string, std::vector<float>> xgd::TextRecognitionNcnnImpl::recognize(
        const cv::Mat &_originImage) {
    cv::Mat srcResize = preProcess(_originImage);
    ncnn::Mat input = ncnn::Mat::from_pixels(
            srcResize.data, ncnn::Mat::PIXEL_GRAY,
            srcResize.cols, srcResize.rows);
    const float mv[3] = {meanValues, meanValues, meanValues}, nv[3] = {normValues, normValues, normValues};
    input.substract_mean_normalize(mv, nv);

    ncnn::Extractor extractor = net->create_extractor();
    extractor.set_num_threads(numThread);
    extractor.input("input", input);
    ncnn::Mat out;
    extractor.extract("out", out);
    return recognize((float *) out.data, out.h, out.w);
}

void xgd::TextRecognitionNcnnImpl::freeModel() {
    net->clear();
    net = nullptr;
}

template<class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::max_element(first, last));
}

std::pair<std::string, std::vector<float>> xgd::TextRecognitionNcnnImpl::recognize(
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

xgd::TextRecognitionNcnnImpl::TextRecognitionNcnnImpl() : numThread(4) {

}

cv::Mat xgd::TextRecognitionNcnnImpl::preProcess(const cv::Mat &_src) {
    cv::Mat srcResized = TextRecognition::preProcess(_src);
    if (srcResized.cols > maxWidth) {
        cv::resize(srcResized, srcResized, cv::Size(maxWidth, dstHeight), 0, 0, cv::INTER_CUBIC);
    }
//    cv::imshow("fuck",srcResized);
//    cv::waitKey(0);
    return srcResized;
}


bool xgd::TextRecognitionOpenCVImpl::initModel(
        const std::string &_onnxFile, const std::string &_words, int _width) {
    try {
        dstWidth = _width;
        model = std::make_shared<cv::dnn::TextRecognitionModel>(_onnxFile);
        model->setDecodeType("CTC-greedy");
        for (auto &c:_words) {
            wordVec.push_back(std::string(1, c));
        }
        model->setVocabulary(wordVec);
        model->setInputParams(normValues, cv::Size(dstWidth, dstHeight), cv::Scalar(meanValues));
    } catch (...) {
        return false;
    }
    return true;
}

void xgd::TextRecognitionOpenCVImpl::freeModel() {
    model = nullptr;
}

std::pair<std::string, std::vector<float>> xgd::TextRecognitionOpenCVImpl::recognize(
        const cv::Mat &_originImage) {
    cv::Mat srcResized = preProcess(_originImage);
    std::string recognitionResult = model->recognize(srcResized);
    std::vector<float> scores(recognitionResult.size(), -1);
    return {recognitionResult, scores};
}

cv::Mat xgd::TextRecognitionOpenCVImpl::preProcess(const cv::Mat &_src) {
    cv::Mat srcResized = TextRecognition::preProcess(_src);
    if (srcResized.cols < dstWidth) {
        cv::hconcat(srcResized, cv::Mat(dstHeight, dstWidth - srcResized.cols, CV_8UC1, cv::Scalar(255)), srcResized);
    } else if (srcResized.cols > dstWidth) {
        cv::resize(srcResized, srcResized, cv::Size(dstWidth, dstHeight), 0, 0, cv::INTER_CUBIC);
    }
    return srcResized;
}

cv::Mat xgd::TextRecognition::preProcess(const cv::Mat &_src) {
    // default behavior: resized to dstHeight
    if (_src.empty())throw std::runtime_error("get empty image in TextRecognition::preProcess");
    float scaleX = (float) dstHeight / (float) _src.rows;
    int dstWidth = int((float) _src.cols * scaleX);
    cv::Mat procImg;
    cv::resize(_src, procImg, cv::Size(dstWidth, dstHeight), 0, 0, cv::INTER_CUBIC);
    return procImg;
}
