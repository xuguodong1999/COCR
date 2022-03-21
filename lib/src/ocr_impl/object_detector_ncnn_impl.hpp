#pragma once

#include "ocr/object_detector.hpp"

#include <opencv2/imgproc.hpp>

#include <ncnn/net.h>
#include <ncnn/datareader.h>

#include <QFile>
#include <QDebug>

#include <string>
#include <memory>
#include <vector>


namespace cocr {
    class ObjectDetectorNcnnImpl : public ObjectDetector {
        int numThread;

        std::shared_ptr<ncnn::Net> net;

    public:
        void setNumThread(int numThread) {
            ObjectDetectorNcnnImpl::numThread = numThread;
            if (net) { net->opt.num_threads = numThread; }
        }

        ObjectDetectorNcnnImpl() : numThread(4), net(nullptr) {

        }

        bool initModel(const std::string &_ncnnBin, const std::string &_ncnnParam, const int &_maxWidth) {
            maxWidth = maxHeight = _maxWidth - sizeBase;
            QFile cfgFile(_ncnnParam.c_str()), weightsFile(_ncnnBin.c_str());
            if (!cfgFile.open(QIODevice::ReadOnly) || !weightsFile.open(QIODevice::ReadOnly)) {
                qDebug() << "fail in QFile read" << _ncnnBin.c_str() << "and" << _ncnnParam.c_str();
                return false;
            }
            QByteArray cfg = cfgFile.readAll();
            cfgFile.close();
            QByteArray weights = weightsFile.readAll();
            weightsFile.close();

            try {
                net = std::make_shared<ncnn::Net>();
                net->opt.num_threads = numThread;
                net->opt.use_vulkan_compute = false;
                net->opt.use_winograd_convolution = true;
                net->opt.use_sgemm_convolution = true;
                net->opt.use_fp16_packed = true;
                net->opt.use_fp16_storage = true;
                net->opt.use_fp16_arithmetic = true;
                net->opt.use_packing_layout = true;
                net->opt.use_shader_pack8 = false;
                net->opt.use_image_storage = false;
                const unsigned char *cfgMem = (const unsigned char *) cfg.data();
                ncnn::DataReaderFromMemory cfgReader(cfgMem);
                int ret_param = net->load_param(cfgReader);
                if (ret_param != 0) {
                    qDebug() << "net->load_param(cfgReader) dies";
                    return false;
                }
                const unsigned char *weightsMem = (const unsigned char *) weights.data();
                ncnn::DataReaderFromMemory weightsReader(weightsMem);
                int ret_bin = net->load_model(weightsReader);
                if (ret_bin != 0) {
                    qDebug() << "net->load_model(weightsReader) dies";
                    return false;
                }

                std::vector<cv::Mat> outs;
                std::vector<int> minSize = {1, 32, 32};
                cv::Mat emptyBlob(3, minSize.data(), CV_8UC1);
                ncnn::Mat in = ncnn::Mat::from_pixels(
                        emptyBlob.data, ncnn::Mat::PIXEL_BGR2RGB,
                        emptyBlob.cols, emptyBlob.rows);
                const float mean_vals[3] = {0, 0, 0};
                const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
                in.substract_mean_normalize(mean_vals, norm_vals);
                ncnn::Extractor ex = net->create_extractor();
                ex.input("data", in);
                ncnn::Mat out;
                ex.extract("output", out);
            } catch (std::exception &e) {
                qDebug() << __FUNCTION__ << "catch" << e.what();
                return false;
            }
            return true;
        }

        void freeModel() override {
            net->clear();
            net = nullptr;
        }


        std::pair<cv::Mat, std::vector<DetectorObject>>
        detect(const cv::Mat &_originImage) override {
            cv::Mat input = preProcess(_originImage);
            ncnn::Mat in = ncnn::Mat::from_pixels(
                    input.data, ncnn::Mat::PIXEL_GRAY,
                    input.cols, input.rows);
            const float mean_vals[3] = {0, 0, 0};
            const float norm_vals[3] = {1 / 255.f, 1 / 255.f, 1 / 255.f};
            in.substract_mean_normalize(mean_vals, norm_vals);
            ncnn::Extractor ex = net->create_extractor();
            ex.input("data", in);
            ncnn::Mat out;
            ex.extract("output", out);
            std::vector<cocr::DetectorObject> objects;
            for (int i = 0; i < out.h; i++) {
                const float *vec = out.row(i);
                int label = vec[0] - 1;
                if (DetectorObject::isValidLabel(label)) {
                    float x = vec[2] * input.cols;
                    float y = vec[3] * input.rows;
                    float w = vec[4] * input.cols - x;
                    float h = vec[5] * input.rows - y;
                    float prob = vec[1];
                    objects.emplace_back(x, y, w, h, label, prob);
                }
            }
            return {input, objects};
        }
    };
}