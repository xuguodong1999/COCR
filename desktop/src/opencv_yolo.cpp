#include "opencv_yolo.hpp"
#include "timer.hpp"
#include "colors.hpp"
#include "std_util.hpp"
#include "opencv_util.hpp"

#include <opencv2/imgproc.hpp>
//#include <opencv2/highgui.hpp>

#include <QApplication>
#include <QFile>

#include <iostream>

using namespace std;

std::vector<std::string> CLASSES = {
        "Br", "O", "I", "S", "H", "N", "C", "B",
        "-", "--", "-+", "=", "F", "#", "Cl", "P", "[o]"};

void OpenCVYolo::setConfThresh(double confThresh) {
    OpenCVYolo::confThresh = confThresh;
}

void OpenCVYolo::setIouThresh(double iouThresh) {
    OpenCVYolo::iouThresh = iouThresh;
}


void OpenCVYolo::init(const char *_cfgPath, const char *_weightsPath) {
    try {
        QFile file(_cfgPath);
        file.open(QIODevice::ReadOnly);
        auto cfgBuffer = file.readAll();
        file.close();
        file.setFileName(_weightsPath);
        file.open(QIODevice::ReadOnly);
        auto weightsBuffer = file.readAll();
        file.close();
        net = cv::dnn::readNetFromDarknet(cfgBuffer.data(), cfgBuffer.length(),
                                          weightsBuffer.data(), weightsBuffer.length());
        // 峰值内存远低于forward时的
        cfgBuffer.clear();
        weightsBuffer.clear();
//        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
//        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        auto outLayers = net.getUnconnectedOutLayers();
        auto layersNames = net.getLayerNames();

        outBlobNames.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); i++) {
            outBlobNames[i] = layersNames[outLayers[i] - 1];
        }
        std::vector<cv::Mat> outs;
        std::vector<int> minSize = {1, 3, 32, 32};
        cv::Mat emptyBlob(4, minSize.data(), CV_32FC3);
        net.setInput(emptyBlob);
        net.forward(outs, outBlobNames);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}


std::pair<std::vector<gt_box>, cv::Mat>
OpenCVYolo::forward(const cv::Mat &_input, bool _debug, const int &_gridSize) {
    int newWidth = _input.cols, newHeight = _input.rows;
    auto roundToGridSize = [&](int &a) -> void {
        if (a % _gridSize != 0) a += (_gridSize - a % _gridSize);
        if (_debug) a += (_gridSize << 1);
    };
    roundToGridSize(newWidth);
    roundToGridSize(newHeight);
    cv::Rect2d outline(0, 0, newWidth, newHeight);

    auto[resizedImg, transInfo] = padCvMatTo(_input, newWidth, newHeight,
                                             getScalar(ColorName::rgbWhite));
    cv::Mat blob;
    cv::dnn::blobFromImage(resizedImg, blob, 1 / 255.0);
    net.setInput(blob);

    std::vector<cv::Mat> outputBlobs;
    Timer timer;
    timer.start();
    net.forward(outputBlobs, outBlobNames);
    timer.stop(true);
    blob.release();

    vector<float> probs;
    vector<cv::Rect2d> boxes;
    vector<int> labels;
    for (size_t i = 0; i < outputBlobs.size(); i++) {
        const auto &vec = outputBlobs[i];
        for (int j = 0; j < vec.rows; j++) {
            double maxProb;
            cv::Point2i maxLoc;
            minMaxLoc(vec.row(j).colRange(5, vec.cols),
                      nullptr, &maxProb, nullptr, &maxLoc);
            if (maxProb < confThresh) continue;
            float cx = vec.at<float>(j, 0) * newWidth;
            float cy = vec.at<float>(j, 1) * newHeight;
            float bw = vec.at<float>(j, 2) * newWidth;
            float bh = vec.at<float>(j, 3) * newHeight;
            cv::Rect2d bBox(cx - bw / 2, cy - bh / 2, bw, bh);
            if (!outline.contains(bBox.tl()) || !outline.contains(bBox.br())) continue;
            boxes.push_back(bBox);
            labels.push_back(maxLoc.x);
            probs.push_back(maxProb);
        }
    }

    std::vector<int> selectedBoxIndices;
    cv::dnn::NMSBoxes(boxes, probs, confThresh, iouThresh, selectedBoxIndices);
    std::vector<gt_box> gtBoxes;
    for (const auto &i:selectedBoxIndices) {
        gtBoxes.emplace_back(boxes[i], labels[i]);
    }
    if (_debug) {
        cv::Mat displayImg = resizedImg.clone();
        auto colorIdx = [](const int &_a) -> ColorName {
            return static_cast<const ColorName>((7 + _a) * 5123 % 455);
        };
        for (const auto &i:selectedBoxIndices) {
            cv::putText(displayImg,
                        CLASSES[labels[i]] + "," + to_string_with_precision(probs[i], 2),
                        boxes[i].tl(), 1, 1.2,
                        getScalar(colorIdx(labels[i])), 2,
                        cv::LINE_AA, false);
            cv::rectangle(displayImg, boxes[i].tl(), boxes[i].br(),
                          getScalar(colorIdx(labels[i])), 1);
        }
//        cv::resize(displayImg, displayImg, cv::Size(displayImg.cols, displayImg.rows) * 2);
//        cv::imshow("YoloForward", displayImg);
//        cv::waitKey(0);
    }
    return {std::move(gtBoxes), std::move(resizedImg)};
}

bool OpenCVYolo::isWeightsLoaded() const {
    return !net.empty();
}
