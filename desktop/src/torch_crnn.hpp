#ifndef _TORCH_CRNN_HPP_
#define _TORCH_CRNN_HPP_

#include "torch_classifier.hpp"

class CRNN : public torch::nn::Module, public BaseClassifier {
public:
    CRNN(const int &_numOfClass, const float &_mv3Scale = 1.0);

    torch::Tensor forward(torch::Tensor x);

    void registerModule();
};

#endif//_TORCH_CRNN_HPP_
