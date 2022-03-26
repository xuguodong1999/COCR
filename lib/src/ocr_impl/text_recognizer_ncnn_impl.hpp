#pragma once

#include "ocr/text_recognizer.hpp"

#include <opencv2/imgproc.hpp>

#include <net.h> // <ncnn/net.h>
#include <datareader.h> // <ncnn/datareader.h>

#include <QFile>
#include <QDebug>

#include <numeric>
#include <string>
#include <memory>
#include <vector>

template<class ForwardIterator>
inline static size_t argmax(ForwardIterator first, ForwardIterator last) {
    return std::distance(first, std::max_element(first, last));
}

namespace cocr {
    class TextRecognizerNcnnImpl : public TextRecognizer {
        int maxWidth;
        int numThread;
        std::shared_ptr<ncnn::Net> net;

        std::pair<std::string, std::vector<float>>
        recognize(const float *_outputData, const int &_h, const int _w) {
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


        cv::Mat preProcess(const cv::Mat &_src) override {
            cv::Mat srcResized = TextRecognizer::preProcess(_src);
            if (srcResized.cols > maxWidth) {
                cv::resize(srcResized, srcResized, cv::Size(maxWidth, dstHeight), 0, 0, cv::INTER_CUBIC);
            }
//    cv::imshow("fuck",srcResized);
//    cv::waitKey(0);
            return srcResized;
        }

    public:
        void setNumThread(int numThread) {
            TextRecognizerNcnnImpl::numThread = numThread;
            if (net) { net->opt.num_threads = numThread; }
        }

        TextRecognizerNcnnImpl() : numThread(16) {

        }

        bool initModel(
                const std::string &_ncnnBin, const std::string &_ncnnParam,
                const std::string &_words, const int &_maxWidth) {
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
            } catch (std::exception &e) {
                qDebug() << __FUNCTION__ << "catch" << e.what();
                return false;
            }
            for (auto &c: _words) {
                wordVec.push_back(std::string(1, c));
            }
            maxWidth = _maxWidth;
            return true;
        }

        std::pair<std::string, std::vector<float>> recognize(const cv::Mat &_originImage) override {
            cv::Mat srcResize = preProcess(_originImage);
            ncnn::Mat in = ncnn::Mat::from_pixels(
                    srcResize.data, ncnn::Mat::PIXEL_GRAY,
                    srcResize.cols, srcResize.rows);
            const float mv[3] = {meanValues, meanValues, meanValues}, nv[3] = {normValues, normValues, normValues};
            in.substract_mean_normalize(mv, nv);

            ncnn::Extractor extractor = net->create_extractor();
            extractor.set_num_threads(numThread);
            extractor.input("input", in);
            ncnn::Mat out;
            extractor.extract("out", out);
            return recognize((float *) out.data, out.h, out.w);
        }

        void freeModel() override {
            net->clear();
            net = nullptr;
        }
    };
}