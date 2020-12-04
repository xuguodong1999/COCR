#include "opencv_yolo.hpp"
#include "timer.hpp"
#include "std_util.hpp"
#include "opencv_util.hpp"

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>

using namespace std;

void OpenCVYolo::init(const char *_cfgPath, const char *_weightsPath) {
    try {
        net = cv::dnn::readNetFromDarknet(_cfgPath, _weightsPath);
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

void OpenCVYolo::forward(const cv::Mat &_input, const int &_gridSize, bool _debug) {
    int width = _input.cols, height = _input.rows;
    if (_debug) {
        width += _gridSize << 1;
        height += _gridSize << 1;
    }
    if (width % _gridSize != 0)
        width = width + _gridSize - width % _gridSize;
    if (height % _gridSize != 0)
        height = height + _gridSize - height % _gridSize;
    auto[resizedImg, offset]=padCvMatTo(_input, width, height, cvWhite);
    auto&[k, offsetX, offsetY]=offset;
    cv::Mat blob;
    cv::dnn::blobFromImage(resizedImg, blob, 1 / 255.0);
    std::cout << blob.size << std::endl;
    net.setInput(blob);
//    Timer timer;
//    timer.start();
    std::vector<cv::Mat> outs;
    net.forward(outs, outBlobNames);
    blob = cv::Mat();// release memory
    vector<float> confidences;
    vector<cv::Rect2d> boxes;
    vector<int> ids;
    double conf_max;
    cv::Point pos_maxconf;
    double conf_thresh = 0.1, iou_thresh = 0.6;
    for (int h = 0; h < outs.size(); h++) {
        const auto &vec = outs[h];
        float *data = (float *) vec.data;
        for (int i = 0; i < vec.rows; i++, data += vec.cols) {
            const auto &posibilities = vec.row(i).colRange(5, vec.cols);
            minMaxLoc(posibilities, 0, &conf_max, 0, &pos_maxconf);
            if (conf_max < conf_thresh)
                continue;
            confidences.push_back(conf_max);
            int _width(data[1 * 2] * resizedImg.cols);
            int _height(data[1 * 3] * resizedImg.rows);
            boxes.push_back(cv::Rect2d(data[0] * resizedImg.cols - _width / 2.,      // centx
                                       data[1] * resizedImg.rows - _height / 2., // centy
                                       _width, _height));
            ids.push_back(pos_maxconf.x);
        }
    }
    vector<int> indices;
    static std::vector<std::string> labels = {"Br", "O", "I", "S", "H", "N",
                                              "C", "B", "-", "--", "##", "=",
                                              "F", "#", "Cl", "P", "[o]"};
    static std::vector<cv::Scalar> colors = {
            cvRosyBrown3, cvSienna1, cvWheat1,
            cvCyan4, cvMidnightBlue, cvGoldenrod2,
            cvSlateBlue, cvKhaki1,
            cvLightGoldenrod4, cvGold1,
            cvRosyBrown1, cvSeaGreen1, cvLightSlateBlue,
            cvVioletRed, cvPurple,
            cvDeepPink1, cvDarkMagenta};

    cv::dnn::NMSBoxes(boxes, confidences, conf_thresh, iou_thresh, indices);
    std::vector<cv::Mat> items;

    for (int i = 0; i < indices.size(); i++) {
//        ids[indices[i]];
//        confidences[indices[i]];
        cv::Rect2d &box = boxes[indices[i]];
        if (box.width < 0 || box.height < 0 ||
            box.x < 0 || box.y < 0 ||
            box.x > resizedImg.cols || box.y > resizedImg.rows) {
            continue;
        }
        items.push_back(resizedImg(box));
        std::cout << labels[ids[indices[i]]] << ",conf=" << confidences[indices[i]] << std::endl;
        std::cout << box << std::endl;
        if (_debug) {
            auto text = labels[ids[indices[i]]] + "," +
                        to_string_with_precision(confidences[indices[i]]);
            cv::putText(resizedImg, text, box.tl(),
                        1, 1.2, colors[ids[indices[i]]], 2,
                        cv::LINE_AA, false);
            cv::rectangle(resizedImg, box.tl(), box.br(),
                          colors[ids[indices[i]]], 1);
        }
    }
//    timer.stop();
    if (_debug) {
        cv::imshow("resizedImg", resizedImg);
        cv::waitKey(0);
    }
}
