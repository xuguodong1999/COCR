#ifndef _YOLO_HPP_
#define _YOLO_HPP_

#include "yolo_object.hpp"
#include <opencv2/core/mat.hpp>
#include <vector>
#include <string>

class YoloDetector {
protected:
    float iouThresh,confThresh;
public:
    YoloDetector():iouThresh(0.6),confThresh(0.15){ }
    float getIouThresh() const;

    void setIouThresh(float iouThresh);

    float getConfThresh() const;

    void setConfThresh(float confThresh);

    virtual bool init(const char *cfgPath, const char *parmPath) = 0;
    /**
     *
     * @param _img 长宽要求为32的倍数
     * @return
     */
    virtual std::vector<YoloObject> detect(const cv::Mat&_img) = 0;

    cv::Mat detectAndDisplay(const cv::Mat&_img,const std::vector<std::string>&_names);
};

float getAvgObjectSize(const std::vector<YoloObject>&_objs);
#endif//_YOLO_HPP_
