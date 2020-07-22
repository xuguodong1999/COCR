/*
南京大学计算机科学与技术系 171860633 徐国栋
Github : https://github.com/Xuguodong1999
Gitlab : https://git.nju.edu.cn/Xuguodong1999
This file is a part of COCR project.
*/
#include "cvutil.h"
#include "colorutil.h"
#include "ioutil.h"

#include <opencv2/opencv.hpp>

#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include <iostream>

using cv::Exception;
using cv::Mat;
using cv::Point;
using cv::waitKey;
using std::cout;
using std::endl;
using std::string;
using std::vector;

static vector<cv::String> outBlobNames;

CVUtil::CVUtil() {
}

bool CVUtil::init(const char *weightsPath, const char *cfgPath, const char *namelistPath) {
    Q_UNUSED(namelistPath);
    QByteArray weights, cfg;
    QFile cf(cfgPath);
    if (!cf.exists()) {
        qDebug()<<cfgPath;
        qDebug() << "!cf.exists()";
        return false;
    }
    cf.open(QIODevice::ReadOnly);
    cfg = cf.readAll();
    cf.close();
    QFile wf(weightsPath);
    if (!wf.exists()) {
        qDebug() << "!wf.exists()";
        return false;
    }
    wf.open(QIODevice::ReadOnly);
    weights = wf.readAll();
    wf.close();
    try {
        net = cv::dnn::readNetFromDarknet(cfg.constData(), cfg.length(), weights.constData(), weights.length());
        net.setPreferableBackend(cv::dnn::DNN_BACKEND_OPENCV);
        net.setPreferableTarget(cv::dnn::DNN_TARGET_CPU);
        auto outLayers = net.getUnconnectedOutLayers();
        auto layersNames = net.getLayerNames();

        outBlobNames.resize(outLayers.size());

        for (size_t i = 0; i < outLayers.size(); ++i)
            outBlobNames[i] = layersNames[outLayers[i] - 1];
    }
    catch (Exception e) {
        qDebug() << "Exception e: " << e.err.c_str();
        return false;
    }
    return true;
}

void CVUtil::detectYolov3(const QImage &image, int topk, float conf_thresh, float iou_thresh, bool isGaussian) {
    int bias1 = 5, bias2 = 1;
    if (isGaussian) {
        bias1 = 9;
        bias2 = 2;
    }
    init();
    frame = ColorUtil::qimage2mat(image);

    // cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    // cv::threshold(frame, frame, 0, 255, cv::THRESH_BINARY | cv::THRESH_OTSU);
    // cv::cvtColor(frame, frame, cv::COLOR_GRAY2BGR);

    // cv::fastNlMeansDenoisingColored(frame, frame, 20,10, 7, 21);

    cv::dnn::blobFromImage(frame, blob, 1 / 255.0,
                           cv::Size(image.width() - image.width() % 32, image.height() - image.height() % 32),
                           cv::Scalar(0, 0, 0), true, false);
    net.setInput(blob);

    vector<Mat> outs;
    /*过模型*/
    time_t start = clock();
    net.forward(outs, outBlobNames);
    qDebug() << "net.forward takes" << (double) (clock() - start) * 1000.0 / (double) CLOCKS_PER_SEC << " ms";

    /*放缩后二次检测*/
    init();
    double k = StdBestDetectWidth / getAverageYoloBoxWidth(frame, outs, bias1, 0.8);
    look(k);
    cv::resize(frame, frame, cv::Size(), k, k);
    cv::dnn::blobFromImage(frame, blob, 1 / 255.0, cv::Size(frame.cols - frame.cols % 32, frame.rows - frame.rows % 32),
                           cv::Scalar(0, 0, 0), true, false);
    net.setInput(blob);

    outs.clear();
    start = clock();
    net.forward(outs, outBlobNames);
    qDebug() << "net.forward takes" << (double) (clock() - start) * 1000.0 / (double) CLOCKS_PER_SEC << " ms";
    /*为NMS收集数据，顺便根据conf_thresh过滤框框*/
    vector<float> confidences;
    vector<cv::Rect2d> boxes;

    vector<int> ids;

    double conf_max;
    cv::Point pos_maxconf;

    for (int h = 0; h < outs.size(); h++) {
        const auto &vec = outs[h];
        float *data = (float *) vec.data;
        for (int i = 0; i < vec.rows; i++, data += vec.cols) {
            const auto &posibilities = vec.row(i).colRange(bias1, vec.cols);
            minMaxLoc(posibilities, 0, &conf_max, 0, &pos_maxconf);
            if (conf_max < conf_thresh)
                continue;
            confidences.push_back(conf_max);
            int width(data[bias2 * 2] * frame.cols);
            int height(data[bias2 * 3] * frame.rows);
            boxes.push_back(cv::Rect2d(data[0] * frame.cols - width / 2,      // centx
                                       data[bias2] * frame.rows - height / 2, // centy
                                       width, height));
            ids.push_back(pos_maxconf.x);
        }
    }
    /*NMS*/
    vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, conf_thresh, iou_thresh, indices);
    objs.resize(indices.size());
    for (size_t i = 0; i < indices.size(); i++) {
        cv::Rect2d &box = boxes[indices[i]];
        objs[i].outline.setRect(box.x, box.y, box.width, box.height);
        objs[i].label = ids[indices[i]];
        objs[i].possibility = confidences[indices[i]];
        // rectangle(
        //	frame, box.tl(), box.br(),
        //	ColorUtil::getColor(objs[i].id, 12),
        //	2
        //);
    }
    //cv::imshow("", frame);
    //cv::waitKey(0);
}

QLineF CVUtil::getLineDirection(const cv::Mat &image3c, const QRectF &qrect) {
    // 如果boxFrame太胖或者太瘦，直接返回端点
    qreal k = qrect.width() / qrect.height();
    const qreal scale = 5;
    if (k > scale || k < 1 / scale) {
        return QLineF(qrect.topLeft(), qrect.bottomRight());
    }
    // 否则，查询boxFrame的四个角的像素密度，暂定检查1/3
    qreal w = qrect.width() / 5, h = qrect.height() / 5;
    const cv::Mat rectFrame = image3c(cv::Rect(qrect.x(), qrect.y(), qrect.width(), qrect.height()));
    float dense[4] = {0, 0, 0, 0};
    for (int ii = 0; ii < 4; ii++) {
        const cv::Mat boxFrame = rectFrame(
                cv::Rect((ii % 2) * (qrect.width() - w), (ii >= 2) * (qrect.height() - h), w, h));
        for (int iii = 0; iii < boxFrame.rows; iii++) {
            for (int jjj = 0; jjj < boxFrame.cols; jjj++) {
                dense[ii] += boxFrame.at<cv::Vec3b>(iii, jjj)[0];
            }
        }
    }
    // note that max pix 255 is color white
    if (dense[0] + dense[3] < dense[1] + dense[2]) {
        if (dense[0] < dense[3])
            return QLineF(qrect.topLeft(), qrect.bottomRight());
        else {
            return QLineF(qrect.bottomRight(), qrect.topLeft());
        }
    } else {
        if (dense[1] < dense[2])
            return QLineF(qrect.topRight(), qrect.bottomLeft());
        else {
            return QLineF(qrect.bottomLeft(), qrect.topRight());
        }
    }
}

const vector<RectObj> &CVUtil::getObjs() {
    return objs;
}

double CVUtil::getAverageYoloBoxWidth(const cv::Mat &frame, const vector<Mat> &outs, int bias, double conf_thresh) {
    double result = 4;
    int count = 1;
    double conf_max;
    cv::Point pos_maxconf;
    for (int h = 0; h < outs.size(); h++) {
        const auto &vec = outs[h];
        float *data = (float *) vec.data;
        for (int i = 0; i < vec.rows; i++, data += vec.cols) {
            const auto &posibilities = vec.row(i).colRange(bias, vec.cols);
            minMaxLoc(posibilities, 0, &conf_max, 0, &pos_maxconf);
            if (conf_max < conf_thresh)
                continue;
            count += 2;
            int width = (int) (data[2] * frame.cols);
            int height = (int) (data[3] * frame.rows);
            result += (width + height);
        }
    }

    return result / count;
}

const cv::Mat CVUtil::getFrame() {
    return frame.clone();
}

RectObj::RectObj() {
}

RectObj::~RectObj() {
}

std::ostream &operator<<(std::ostream &out, const RectObj &obj) {
    out << "(" << obj.label << "," << obj.possibility << ")";
    return out;
}
