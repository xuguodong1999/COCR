#ifndef _MOBILENETV3_HPP_
#define _MOBILENETV3_HPP_

#include "config.hpp"
#include <torch/nn.h>


class Mv3Small : public torch::nn::Module {
    torch::nn::Sequential layerIn;
    std::vector<torch::nn::Sequential> bnecks;
    torch::nn::Sequential layerOut;
public:
    Mv3Small(int _numOfClass);

    torch::Tensor forward(torch::Tensor x);
};

class Mv3Large : public torch::nn::Module {
    torch::nn::Sequential layerIn;
    std::vector<torch::nn::Sequential> bnecks;
    torch::nn::Sequential layerOut;
public:
    Mv3Large(int _numOfClass);

    torch::Tensor forward(torch::Tensor x);
};

#endif//_MOBILENETV3_HPP_
