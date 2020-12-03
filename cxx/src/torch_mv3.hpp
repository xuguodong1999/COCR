#ifndef _TORCH_MV3_HPP_
#define _TORCH_MV3_HPP_

#include "torch_classifier.hpp"
#include <vector>

class Mv3Small : public torch::nn::Module, public BaseClassifier {
public:
    Mv3Small(const int &_numOfClass);

    torch::Tensor forward(torch::Tensor x);

    void registerModule();
};

class Mv3Large : public torch::nn::Module, public BaseClassifier {
public:
    Mv3Large(const int &_numOfClass, const float &_mv3Scale = 1.0);

    torch::Tensor forward(torch::Tensor x);

    void registerModule();
};

#endif//_TORCH_MV3_HPP_
