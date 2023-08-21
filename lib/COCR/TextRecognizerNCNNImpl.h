#pragma once

#include "XGD/COCR/TextRecognizer.h"
#include "NCNNUtil.h"

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
            float partition = std::accumulate(exps.begin(), exps.end(), 0.0f);//row sum
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


    Mat preProcess(const Mat &_src) override {
        Mat srcResized = TextRecognizer::preProcess(_src);
        if (srcResized.getWidth() > maxWidth) {
            return CvUtil::Resize(srcResized, {maxWidth, dstHeight});
        }
        return srcResized;
    }

public:
    void setNumThread(int numThread) {
        TextRecognizerNcnnImpl::numThread = numThread;
        if (net) { net->opt.num_threads = numThread; }
    }

    TextRecognizerNcnnImpl() : numThread(4) {

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
            configNcnnNet(*net);
            const unsigned char *cfgMem = (const unsigned char *) cfg.data();
            ncnn::DataReaderFromMemory cfgReader(cfgMem);
            int ret_param = net->load_param(cfgReader);
            if (ret_param != 0) {
                qDebug() << "net->load_param(cfgReader) failed";
                return false;
            }
            const unsigned char *weightsMem = (const unsigned char *) weights.data();
            ncnn::DataReaderFromMemory weightsReader(weightsMem);
            int ret_bin = net->load_model(weightsReader);
            if (ret_bin != 0) {
                qDebug() << "net->load_model(weightsReader) failed";
                return false;
            }
        } catch (std::exception &e) {
            qDebug() << __FUNCTION__ << "catch" << e.what();
            return false;
        }
        for (auto &c: _words) {
            wordVec.emplace_back(1, c);
        }
        maxWidth = _maxWidth;
        return true;
    }

    std::pair<std::string, std::vector<float>> recognize(const Mat &_originImage) override {
        Mat srcResize = preProcess(_originImage);
        ncnn::Mat in = ncnn::Mat::from_pixels(
                srcResize.getData(), ncnn::Mat::PIXEL_GRAY,
                srcResize.getWidth(), srcResize.getHeight());
        const float mv[3] = {meanValues, meanValues, meanValues}, nv[3] = {normValues, normValues, normValues};
        in.substract_mean_normalize(mv, nv);

        ncnn::Extractor extractor = net->create_extractor();
        extractor.set_num_threads(numThread);
        extractor.input("in0", in);
        ncnn::Mat out;
        extractor.extract("out0", out);
        return recognize((float *) out.data, out.h, out.w);
    }

    void freeModel() override {
        net->clear();
        net = nullptr;
    }
};
