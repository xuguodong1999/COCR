#ifndef _TORCH_YOLOV4_HPP_
#define _TORCH_YOLOV4_HPP_

#include "torch_classifier.hpp"

class Yolov4 : public torch::nn::Module, public BaseClassifier {
    std::unordered_map<std::string, size_t> mDetectionMap;

public:
    Yolov4(const int&_numOfClass,const float &_mv3Scale = 1.0);

    void registerModule() override;
};

#endif//_TORCH_YOLOV4_HPP_
