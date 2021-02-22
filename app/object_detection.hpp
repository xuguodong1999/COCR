#ifndef _XGD_OBJECT_DETECTION_HPP_
#define _XGD_OBJECT_DETECTION_HPP_

#include <opencv2/core/mat.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <memory>
#include <vector>
namespace xgd {
    struct DetectorObject {
        float x, y, w, h, prob;
        int label;
        DetectorObject(const float &_x, const float &_y, const float &_w, const float &_h,
                       const int &_label, const float &_prob = 1);
    };

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

        virtual std::pair<cv::Mat,std::vector<DetectorObject>>
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

        std::pair<cv::Mat,std::vector<DetectorObject>>
        detect(const cv::Mat &_originImage);
    };
}
#endif//_XGD_OBJECT_DETECTION_HPP_
