#pragma once

#include "els_nn_export.h"

#include "nn/classifier.h"

class ELS_NN_EXPORT CRNN : public torch::nn::Module, public BaseClassifier {
public:
    CRNN(const int &_numOfClass, const float &_mv3Scale = 1.0);

    torch::Tensor forward(torch::Tensor x);

    void registerModule();
};