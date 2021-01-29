#include "yolo_opencv.hpp"

#include "colors.hpp"
#include "std_util.hpp"
#include "opencv_util.hpp"

#include <opencv2/imgproc.hpp>

#include <QApplication>
#include <QFile>

#include <iostream>

using namespace std;

bool YoloOpenCVImpl::init(const char *cfgPath, const char *parmPath) {
    try {
        QFile file(cfgPath);
        file.open(QIODevice::ReadOnly);
        auto cfgBuffer = file.readAll();
        file.close();
        file.setFileName(parmPath);
        file.open(QIODevice::ReadOnly);
        auto weightsBuffer = file.readAll();
        file.close();
        net = cv::dnn::readNetFromDarknet(cfgBuffer.data(), cfgBuffer.length(),
                                          weightsBuffer.data(), weightsBuffer.length());
        cfgBuffer.clear();
        weightsBuffer.clear();
//        net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
//        net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
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
        return false;
    }
    return true;
}

std::vector<YoloObject> YoloOpenCVImpl::detect(const cv::Mat &_img) {
    cv::Mat blob;
    cv::dnn::blobFromImage(_img, blob, 1 / 255.0);
    net.setInput(blob);
    std::vector<cv::Mat> outputBlobs;
    net.forward(outputBlobs, outBlobNames);
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
            float cx = vec.at<float>(j, 0) * _img.cols;
            float cy = vec.at<float>(j, 1) * _img.rows;
            float bw = vec.at<float>(j, 2) * _img.cols;
            float bh = vec.at<float>(j, 3) * _img.rows;
            boxes.emplace_back(cx - bw / 2, cy - bh / 2, bw, bh);
            labels.push_back(maxLoc.x);
            probs.push_back(maxProb);
        }
    }
    std::vector<int> selectedBoxIndices;
    cv::dnn::NMSBoxes(boxes, probs, confThresh, iouThresh, selectedBoxIndices);
    std::vector<YoloObject> gtBoxes;
    for (const auto &i:selectedBoxIndices) {
        gtBoxes.emplace_back(boxes[i].x, boxes[i].y, boxes[i].width, boxes[i].height,
                             labels[i], probs[i]);
    }
    return std::move(gtBoxes);
}
