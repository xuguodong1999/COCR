// Copyright 2020-present pytorch-cpp Authors
#pragma once

#include <torch/types.h>
#include <torch/nn/modules/rnn.h>
#include <torch/nn/modules/embedding.h>
#include <torch/nn/module.h>
#include <torch/nn/modules/linear.h>
#include <tuple>

class RNNLMImpl : public torch::nn::Module {
public:
    RNNLMImpl(int64_t vocab_size, int64_t embed_size, int64_t hidden_size, int64_t num_layers);

    std::tuple<torch::Tensor, std::tuple<torch::Tensor, torch::Tensor>> forward(torch::Tensor x,
                                                                                std::tuple<torch::Tensor, torch::Tensor> hx);

private:
    torch::nn::Embedding embed;
    torch::nn::LSTM lstm;
    torch::nn::Linear linear;
};

TORCH_MODULE(RNNLM);
