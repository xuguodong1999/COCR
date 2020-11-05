#ifndef _MODULE_HPP_
#define _MODULE_HPP_

#include "config.hpp"
#include "activation.hpp"
#include <torch/nn.h>

class SeModuleImpl : public torch::nn::Module {
    torch::nn::Sequential se;
//    torch::nn::AdaptiveAvgPool2d a;
//    torch::nn::Conv2d b;
//    torch::nn::BatchNorm2d c;
//    torch::nn::ReLU d;
//    torch::nn::Conv2d e;
//    torch::nn::BatchNorm2d f;
//    HSigmoid g;
public:
    SeModuleImpl(int in_size, int reduction = 4);

    torch::Tensor forward(torch::Tensor x);
};


TORCH_MODULE(SeModule);

class BlockHSwishModuleImpl : public torch::nn::Module {
    int stride_;
    int in_size_;
    int out_size_;
    torch::nn::Conv2d conv1;
    torch::nn::Conv2d conv2;
    torch::nn::Conv2d conv3;
    HSwish nolinear1;
    HSwish nolinear2;
    torch::nn::BatchNorm2d bn1;
    torch::nn::BatchNorm2d bn2;
    torch::nn::BatchNorm2d bn3;
    SeModule se;
    torch::nn::Sequential shortcut;
public:
    BlockHSwishModuleImpl(int kernel_size, int in_size, int expand_size, int out_size,
                          HSwish nolinear, SeModule semodule, int stride);

    torch::Tensor forward(torch::Tensor x);
};

TORCH_MODULE(BlockHSwishModule);

class BlockHSwishNullModuleImpl : public torch::nn::Module {
    int stride_;
    int in_size_;
    int out_size_;
    torch::nn::Conv2d conv1;
    torch::nn::Conv2d conv2;
    torch::nn::Conv2d conv3;
    HSwish nolinear1;
    HSwish nolinear2;
    torch::nn::BatchNorm2d bn1;
    torch::nn::BatchNorm2d bn2;
    torch::nn::BatchNorm2d bn3;

    torch::nn::Sequential shortcut;
public:
    BlockHSwishNullModuleImpl(int kernel_size, int in_size, int expand_size, int out_size,
                              HSwish nolinear, int stride);

    torch::Tensor forward(torch::Tensor x);
};

TORCH_MODULE(BlockHSwishNullModule);

class BlockReLUModuleImpl : public torch::nn::Module {
    int stride_;
    int in_size_;
    int out_size_;
    torch::nn::Conv2d conv1;
    torch::nn::Conv2d conv2;
    torch::nn::Conv2d conv3;
    torch::nn::ReLU nolinear1;
    torch::nn::ReLU nolinear2;
    torch::nn::BatchNorm2d bn1;
    torch::nn::BatchNorm2d bn2;
    torch::nn::BatchNorm2d bn3;
    SeModule se;
    torch::nn::Sequential shortcut;
public:
    BlockReLUModuleImpl(int kernel_size, int in_size, int expand_size, int out_size,
                        torch::nn::ReLU nolinear, SeModule semodule, int stride);

    torch::Tensor forward(torch::Tensor x);

};

TORCH_MODULE(BlockReLUModule);

class BlockReLUNullModuleImpl : public torch::nn::Module {
    int stride_;
    int in_size_;
    int out_size_;
    torch::nn::Conv2d conv1;
    torch::nn::Conv2d conv2;
    torch::nn::Conv2d conv3;
    torch::nn::ReLU nolinear1;
    torch::nn::ReLU nolinear2;
    torch::nn::BatchNorm2d bn1;
    torch::nn::BatchNorm2d bn2;
    torch::nn::BatchNorm2d bn3;
    torch::nn::Sequential shortcut;
public:
    BlockReLUNullModuleImpl(int kernel_size, int in_size, int expand_size, int out_size,
                            torch::nn::ReLU nolinear, int stride);

    torch::Tensor forward(torch::Tensor x);

};

TORCH_MODULE(BlockReLUNullModule);

#endif //_MODULE_HPP_
