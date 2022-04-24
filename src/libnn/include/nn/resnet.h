// Copyright 2020-present pytorch-cpp Authors
#pragma once
#include "els_nn_export.h"
#include <torch/types.h>
#include <torch/nn/module.h>
#include <torch/nn/modules/container/sequential.h>
#include <torch/nn/modules/activation.h>
#include <torch/nn/modules/batchnorm.h>
#include <torch/nn/modules/conv.h>
#include <torch/nn/modules/linear.h>
#include <vector>
#include <torch/nn/modules/pooling.h>

class ELS_NN_EXPORT ResNet : public torch::nn::Module {
public:
    explicit ResNet(const std::array<int64_t, 3> &layers, int64_t num_classes = 10);

    torch::Tensor forward(torch::Tensor x);

private:
    int64_t in_channels;
    torch::nn::Conv2d conv;
    torch::nn::BatchNorm2d bn;
    torch::nn::ReLU relu;
    torch::nn::Sequential layer1;
    torch::nn::Sequential layer2;
    torch::nn::Sequential layer3;
    torch::nn::AvgPool2d avg_pool;
    torch::nn::Linear fc;

    torch::nn::Sequential make_layer(int64_t out_channels, int64_t blocks, int64_t stride = 1);
};