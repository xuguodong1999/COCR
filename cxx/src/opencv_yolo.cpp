#include "opencv_yolo.hpp"
#include "timer.hpp"
#include "opencv_util.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

using namespace std;

void OpenCVYolo::init(const char *_cfgPath, const char *_weightsPath) {
    try {
        net = cv::dnn::readNetFromDarknet(_cfgPath, _weightsPath);
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        auto outLayers = net.getUnconnectedOutLayers();
        auto layersNames = net.getLayerNames();
        outBlobNames.resize(outLayers.size());
        for (size_t i = 0; i < outLayers.size(); i++) {
            outBlobNames[i] = layersNames[outLayers[i] - 1];
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}

void OpenCVYolo::forward(const cv::Mat &_input, const int &_gridSize) {
    int width = _input.cols, height = _input.rows;
    if (width % _gridSize != 0)
        width = width + _gridSize - width % _gridSize;
    if (height % _gridSize != 0)
        height = height + _gridSize - height % _gridSize;
    auto[resizedImg, offset]=padCvMatTo(_input, width, height, cvWhite);
    auto&[k, offsetX, offsetY]=offset;
    cv::Mat blob;
    cv::dnn::blobFromImage(resizedImg, blob, 1 / 255.0);
    net.setInput(blob);
    vector<cv::Mat> outs;
    net.forward(outs, outBlobNames);
    blob = cv::Mat();// release memory
    vector<float> confidences;
    vector<cv::Rect2d> boxes;
    vector<int> ids;
    double conf_max;
    cv::Point pos_maxconf;
    float conf_thresh = 0.1, iou_thresh = 0.45;
    for (int h = 0; h < outs.size(); h++) {
        const auto &vec = outs[h];
        float *data = (float *) vec.data;
        for (int i = 0; i < vec.rows; i++, data += vec.cols) {
            const auto &posibilities = vec.row(i).colRange(5, vec.cols);
            minMaxLoc(posibilities, 0, &conf_max, 0, &pos_maxconf);
            if (conf_max < conf_thresh)
                continue;
            confidences.push_back(conf_max);
            int width(data[1 * 2] * resizedImg.cols);
            int height(data[1 * 3] * resizedImg.rows);
            boxes.push_back(cv::Rect2d(data[0] * resizedImg.cols - width / 2,      // centx
                                       data[1] * resizedImg.rows - height / 2, // centy
                                       width, height));
            ids.push_back(pos_maxconf.x);
        }
    }
    vector<int> indices;
    static std::vector<std::string> labels = {"Br", "O", "I", "S", "H", "N",
                                              "C", "B", "-", "--", "##", "=",
                                              "F", "#", "Cl", "P", "[o]"};
    cv::dnn::NMSBoxes(boxes, confidences, conf_thresh, iou_thresh, indices);
    for (size_t i = 0; i < indices.size(); i++) {
        cv::Rect2d &box = boxes[indices[i]];
//        ids[indices[i]];
//        confidences[indices[i]];
        std::cout << labels[ids[indices[i]]] << ",conf=" << confidences[indices[i]] << std::endl;
        rectangle(
                resizedImg, box.tl(), box.br(),
                cvBlue,
                2
        );
    }
    cv::imshow("resizedImg", resizedImg);
    cv::waitKey(0);
}
