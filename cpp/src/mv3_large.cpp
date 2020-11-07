#include "mv3.hpp"
#include "activation.hpp"
#include "module.hpp"

Mv3Large::Mv3Large(int _numOfClass)
        : layerIn(torch::nn::Sequential(
        torch::nn::Conv2d(
                torch::nn::Conv2dOptions(3, 16, {3, 3})
                        .stride({2, 2})
                        .padding({1, 1})
                        .bias(false)),
        torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(16)),
        HSwish())),
          bnecks({torch::nn::Sequential(
                  Mv3BneckModule(16, 16, 16, 3, 1, Mv3Activation::Relu, false),
                  Mv3BneckModule(16, 64, 24, 3, 2, Mv3Activation::Relu, false)),
                  torch::nn::Sequential(
                          Mv3BneckModule(24, 72, 24, 3, 1, Mv3Activation::Relu, false),
                          Mv3BneckModule(24, 72, 40, 5, 2, Mv3Activation::Relu, true)),
                  torch::nn::Sequential(
                          Mv3BneckModule(40, 120, 40, 5, 1, Mv3Activation::Relu, true),
                          Mv3BneckModule(40, 120, 40, 5, 1, Mv3Activation::Relu, true),
                          Mv3BneckModule(40, 240, 80, 3, 2, Mv3Activation::HSwish, false)),
                  torch::nn::Sequential(
                          Mv3BneckModule(80, 200, 80, 3, 1, Mv3Activation::HSwish, false),
                          Mv3BneckModule(80, 184, 80, 3, 1, Mv3Activation::HSwish, false),
                          Mv3BneckModule(80, 184, 80, 3, 1, Mv3Activation::HSwish, false),
                          Mv3BneckModule(80, 480, 112, 3, 1, Mv3Activation::HSwish, true),
                          Mv3BneckModule(112, 672, 112, 3, 1, Mv3Activation::HSwish, true),
                          Mv3BneckModule(112, 672, 160, 5, 2, Mv3Activation::HSwish, true)),
                  torch::nn::Sequential(
                          Mv3BneckModule(160, 960, 160, 5, 1, Mv3Activation::HSwish, true),
                          Mv3BneckModule(160, 960, 160, 5, 1, Mv3Activation::HSwish, true))}
          ),
          layerOut(torch::nn::Sequential(
                  // part1: fc1
                  torch::nn::Conv2d(
                          torch::nn::Conv2dOptions(
                                  160, 960, {1, 1})
                                  .bias(false)),
                  torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(960)),
                  HSwish(),
                  // part2: apply avg pool
                  torch::nn::AdaptiveAvgPool2d(torch::nn::AdaptiveAvgPool2dOptions({1, 1})),
                  // part3: fc2
                  torch::nn::Conv2d(
                          torch::nn::Conv2dOptions(
                                  960, 1280, {1, 1})
                                  .bias(true)),
                  HSwish(),
                  // part4: 到节点
                  torch::nn::Conv2d(torch::nn::Conv2dOptions(
                          1280, _numOfClass, {1, 1}).bias(true))
          )) {
    register_module("conv2d-in", layerIn);
    for (size_t i = 0; i < bnecks.size(); i++) {
        register_module("bneck-" + std::to_string(i), bnecks[i]);
    }
    register_module("fc-layer", layerOut);
}

torch::Tensor Mv3Large::forward(torch::Tensor x) {
    x = layerIn->forward(x);
    for (auto &bneck:bnecks) {
        x = bneck->forward(x);
    }
    x = layerOut->forward(x);
    x = x.squeeze(-1);
    x = x.squeeze(-1);
    return torch::log_softmax(x, 1);
}
