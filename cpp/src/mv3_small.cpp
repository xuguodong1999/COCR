#include "mv3.hpp"
#include "activation.hpp"
#include "module.hpp"

Mv3Small::Mv3Small(int _numOfClass)
        : layerIn(torch::nn::Sequential(
        torch::nn::Conv2d(
                torch::nn::Conv2dOptions(3, 16, {3, 3})
                        .stride({2, 2})
                        .padding({1, 1})
                        .bias(false)),
        torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(16)),
        HSwish())),
          bnecks({torch::nn::Sequential(
                  Mv3BneckModule(16, 16, 16, 3, 2, Mv3Activation::Relu, true)),

                  torch::nn::Sequential(
                          Mv3BneckModule(16, 72, 24, 3, 2, Mv3Activation::Relu, false)),

                  torch::nn::Sequential(
                          Mv3BneckModule(24, 88, 24, 3, 1, Mv3Activation::Relu, false),
                          Mv3BneckModule(24, 96, 40, 5, 2, Mv3Activation::HSwish, true)),

                  torch::nn::Sequential(
                          Mv3BneckModule(40, 240, 40, 5, 1, Mv3Activation::HSwish, true),
                          Mv3BneckModule(40, 240, 40, 5, 1, Mv3Activation::HSwish, true),
                          Mv3BneckModule(40, 120, 48, 5, 1, Mv3Activation::HSwish, true),
                          Mv3BneckModule(48, 144, 48, 5, 1, Mv3Activation::HSwish, true),
                          Mv3BneckModule(48, 288, 96, 5, 2, Mv3Activation::HSwish, true)),

                  torch::nn::Sequential(
                          Mv3BneckModule(96, 576, 96, 5, 1, Mv3Activation::HSwish, true),
                          Mv3BneckModule(96, 576, 96, 5, 1, Mv3Activation::HSwish, true))}
          ),
          layerOut(torch::nn::Sequential(
                  // part1: fc1
                  torch::nn::Conv2d(
                          torch::nn::Conv2dOptions(
                                  96, 576, {1, 1})
                                  .bias(false)),
                  torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(576)),
                  HSwish(),
                  // part2: apply avg pool
                  torch::nn::AdaptiveAvgPool2d(torch::nn::AdaptiveAvgPool2dOptions({1, 1})),
                  // part3: fc2
                  torch::nn::Conv2d(
                          torch::nn::Conv2dOptions(
                                  576, 1024, {1, 1})
                                  .bias(true)),
                  HSwish(),
                  // part4: 到节点
                  torch::nn::Conv2d(torch::nn::Conv2dOptions(
                          1024, _numOfClass, {1, 1}).bias(true))
          )) {
    register_module("conv2d-in", layerIn);
    for (size_t i = 0; i < bnecks.size(); i++) {
        register_module("bneck-" + std::to_string(i), bnecks[i]);
    }
    register_module("fc-layer", layerOut);
}

torch::Tensor Mv3Small::forward(torch::Tensor x) {
    x = layerIn->forward(x);
    for (auto &bneck:bnecks) {
        x = bneck->forward(x);
    }
    x = layerOut->forward(x);
    x = x.squeeze(-1);
    x = x.squeeze(-1);
    return torch::log_softmax(x, 1);
}
