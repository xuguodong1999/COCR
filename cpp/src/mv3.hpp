#ifndef _MOBILENETV3_HPP_
#define _MOBILENETV3_HPP_

#include "config.hpp"
#include "module.hpp"


class Mv3Small : public torch::nn::Module {
    torch::nn::Conv2d conv_in;
    torch::nn::Conv2d conv_bneck_out;
    torch::nn::BatchNorm2d bn_in;
    torch::nn::BatchNorm2d bn_bneck_out;
//    torch::nn::BatchNorm2d bn_fc1;
    torch::nn::Conv2d conv_fc1;
    torch::nn::Conv2d conv_fc2;
    HSwish act_in;
    HSwish act_bneck_out;
    HSwish act_fc1;
    torch::nn::Sequential bneck;
public:
    Mv3Small(int num_classes = 1000);

    torch::Tensor forward(torch::Tensor x);

};

class Mv3Large : public torch::nn::Module {
    torch::nn::Sequential layerIn;
    torch::nn::Sequential bneck;
    torch::nn::Sequential layerOut;
public:
    Mv3Large(int numOfClass);

    torch::Tensor forward(torch::Tensor x);
};

#endif//_MOBILENETV3_HPP_
