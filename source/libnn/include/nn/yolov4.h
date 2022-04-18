#pragma once

#include "els_nn_export.h"

#include "nn/classifier.h"

class ELS_NN_EXPORT Yolov4 : public torch::nn::Module, public BaseClassifier {
    std::unordered_map<std::string, torch::nn::Sequential> prnLayerMap;
    torch::nn::Sequential &h32x;    // 指向 BaseClassifier::layerOut
    torch::nn::Sequential h16x, h8x;
public:
    Yolov4(const int &_numOfClass, const float &_mv3Scale = 1.0);

    void registerModule() override;

    torch::Tensor forward(torch::Tensor x) override;

    torch::Tensor forwardNoGardWithNMS(torch::Tensor input);

    void saveClassifier(const std::string &_saveDir,
                        const std::string &_inPrefix = "in",
                        const std::string &_bneckPrefix = "bneck",
                        const std::string &_outPrefix = "detector") override;

};
