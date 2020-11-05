#ifndef _MOBILENETV3_HPP_
#define _MOBILENETV3_HPP_

#include "config.hpp"

#include <torch/torch.h>

class HSwishImpl : public torch::nn::Module {
public:
    HSwishImpl() = default;

    torch::Tensor forward(torch::Tensor x);

};

TORCH_MODULE(HSwish);

class HSigmoidImpl : public torch::nn::Module {
public:
    HSigmoidImpl() = default;

    torch::Tensor forward(torch::Tensor x);
};


TORCH_MODULE(HSigmoid);

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

class BlockHSwishModule : public torch::nn::ModuleHolder<BlockHSwishModuleImpl> {
public:
    using torch::nn::ModuleHolder<BlockHSwishModuleImpl>::ModuleHolder;
};

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

class BlockHSwishNullModule : public torch::nn::ModuleHolder<BlockHSwishNullModuleImpl> {
public:
    using torch::nn::ModuleHolder<BlockHSwishNullModuleImpl>::ModuleHolder;
};

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

class BlockReLUModule : public torch::nn::ModuleHolder<BlockReLUModuleImpl> {
public:
    using torch::nn::ModuleHolder<BlockReLUModuleImpl>::ModuleHolder;
};

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

class BlockReLUNullModule : public torch::nn::ModuleHolder<BlockReLUNullModuleImpl> {
public:
    using torch::nn::ModuleHolder<BlockReLUNullModuleImpl>::ModuleHolder;
};

class Mv3Small : public torch::nn::Module {
    torch::nn::Conv2d conv_in;
    torch::nn::Conv2d conv_bneck_out;
    torch::nn::BatchNorm2d bn_in;
    torch::nn::BatchNorm2d bn_bneck_out;
    torch::nn::BatchNorm2d bn_fc1;
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
    std::vector<torch::nn::Sequential> bnecks;
    torch::nn::Sequential layerOut;
public:
    Mv3Large(int numOfClass);

    torch::Tensor forward(torch::Tensor x) ;
};

#endif//_MOBILENETV3_HPP_
