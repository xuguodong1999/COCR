#include "mv3.hpp"

Mv3Large::Mv3Large(int numOfClass)
        : layerIn(torch::nn::Sequential(
        torch::nn::Conv2d(torch::nn::Conv2dOptions(3, 16, {3, 3})
                                  .stride({2, 2})
                                  .padding({1, 1})
                                  .bias(false)),
        torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(16)),
        HSwish()
)),
          bneck(torch::nn::Sequential(
                  BlockReLUNullModule(
                          3, 16, 16, 16,
                          torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
                          1),
                  BlockReLUNullModule(
                          3, 16, 64, 24,
                          torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
                          2),
                  BlockReLUNullModule(
                          3, 24, 72, 24,
                          torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
                          1),
                  ////////////////////////////
                  // ①
                  ////////////////////////////
                  BlockReLUModule(
                          5, 24, 72, 24,
                          torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
                          SeModule(24),
                          2),
                  BlockReLUModule(
                          5, 24, 120, 40,
                          torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
                          SeModule(40),
                          1),
                  BlockReLUModule(
                          5, 40, 120, 40,
                          torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
                          SeModule(40),
                          1),
                  ////////////////////////////
                  // ②
                  ////////////////////////////
                  BlockHSwishNullModule(
                          3, 40, 240, 80,
                          HSwish(),
                          2),
                  BlockHSwishNullModule(
                          3, 80, 200, 80,
                          HSwish(),
                          1),
                  BlockHSwishNullModule(
                          3, 80, 184, 80,
                          HSwish(),
                          1),
                  BlockHSwishNullModule(
                          3, 80, 184, 80,
                          HSwish(),
                          1),
                  ////////////////////////////
                  // ③
                  ////////////////////////////
                  BlockHSwishModule(
                          3, 80, 480, 112,
                          HSwish(),
                          SeModule(112),
                          2),
                  BlockHSwishModule(
                          3, 112, 762, 112,
                          HSwish(),
                          SeModule(112),
                          1),
                  ////////////////////////////
                  // ③-①
                  ////////////////////////////
                  BlockHSwishModule(
                          5, 112, 672, 160,
                          HSwish(),
                          SeModule(160),
                          1),
                  BlockHSwishModule(
                          5, 160, 960, 160,
                          HSwish(),
                          SeModule(160),
                          1),
                  BlockHSwishModule(
                          5, 160, 960, 160,
                          HSwish(),
                          SeModule(160),
                          1)
          )),
          layerOut(torch::nn::Sequential(
                  // part1: fc1
                  torch::nn::Conv2d(torch::nn::Conv2dOptions(
                          160, 960, {1, 1}).bias(false)),
                  torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(960)),
                  HSwish(),
                  // part2: apply avg pool
                  torch::nn::AdaptiveAvgPool2d(torch::nn::AdaptiveAvgPool2dOptions({1, 1})),
                  // part3: fc2
                  torch::nn::Conv2d(torch::nn::Conv2dOptions(
                          960, 1280, {1, 1}).bias(true)),
                  HSwish(),
                  // part4: 到节点
                  torch::nn::Conv2d(torch::nn::Conv2dOptions(
                          1280, numOfClass, {1, 1}).bias(true))
          )) {
    register_module("layerIn", layerIn);
    register_module("bneck", bneck);
    register_module("layerOut", layerOut);
}

torch::Tensor Mv3Large::forward(torch::Tensor x) {
    x = layerIn->forward(x);
    x = bneck->forward(x);
    x = layerOut->forward(x);
    x = x.squeeze(-1);
    x = x.squeeze(-1);
    return torch::log_softmax(x, 1);
}
