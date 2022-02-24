// Copyright 2020-present pytorch-cpp Authors
#pragma once

#include <torch/types.h>
#include <torch/nn/module.h>
#include <torch/nn/modules/linear.h>

class NeuralNetImpl : public torch::nn::Module {
public:
    NeuralNetImpl(int64_t input_size, int64_t hidden_size, int64_t num_classes);

    torch::Tensor forward(torch::Tensor x);

private:
    torch::nn::Linear fc1;
    torch::nn::Linear fc2;
};

TORCH_MODULE(NeuralNet);
