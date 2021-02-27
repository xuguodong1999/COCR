#ifndef _XGD_OBJECT_DETECTION_HPP_
#define _XGD_OBJECT_DETECTION_HPP_

#include "ocr_types.hpp"
#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <memory>
#include <vector>

namespace ncnn {
    class Net;
}

namespace xgd {
    // FIXME: 添加对宽度小于1的边框的优化
    class ObjectDetector {
    protected:
        bool isInited;
        std::vector<std::string> objectNames;
        const float meanValues = 127.5, normValues = 1.0 / 127.5;
        const int sizeBase = 32;
        int maxWidth, maxHeight;

        /**
         * 默认行为：转单通道，边长向上转 sizeBase 的倍数，边长限制到 [maxWidth,maxHeight]
         * @param _src
         * @return 可以送进网络的图像
         */
        virtual cv::Mat preProcess(const cv::Mat &_src);

    public:
        ObjectDetector();

        virtual void freeModel() = 0;

        virtual std::pair<cv::Mat, std::vector<DetectorObject>>
        detect(const cv::Mat &_originImage) = 0;
    };

    class ObjectDetectorOpenCVImpl : public ObjectDetector {
        cv::dnn::Net net;
        std::vector<cv::String> outBlobNames;
        float confThresh, iouThresh;
    public:
        void setConfThresh(float confThresh);

        void setIouThresh(float iouThresh);

    public:
        bool initModel(const std::string &_cfgFile, const std::string &_weightsFile);

        void freeModel();

        std::pair<cv::Mat, std::vector<DetectorObject>>
        detect(const cv::Mat &_originImage);
    };

    class ObjectDetectorNCNNImpl : public ObjectDetector {
        int numThread;
    public:
        void setNumThread(int numThread);

    private:
        std::shared_ptr<ncnn::Net> net;

    public:
        ObjectDetectorNCNNImpl();

        bool initModel(const std::string &_ncnnBin, const std::string &_ncnnParam, const int &_maxWidth);

        void freeModel();

        std::pair<cv::Mat, std::vector<DetectorObject>>
        detect(const cv::Mat &_originImage);
    };
}
#endif//_XGD_OBJECT_DETECTION_HPP_
