#pragma once

#include "els_nn_export.h"

#include "nn/classifier.h"
#include <vector>

class ELS_NN_EXPORT Mv3Small : public torch::nn::Module, public BaseClassifier {
public:
    Mv3Small(const int &_numOfClass);

    torch::Tensor forward(torch::Tensor x);

    void registerModule();
};

class ELS_NN_EXPORT Mv3Large : public torch::nn::Module, public BaseClassifier {
public:
    Mv3Large(const int &_numOfClass, const float &_mv3Scale = 1.0);

    torch::Tensor forward(torch::Tensor x);

    void registerModule();
};
