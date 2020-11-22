#include "torch_mv3.hpp"
#include "torch_activation.hpp"
#include "torch_module.hpp"
#include <torch/torch.h>

using namespace torch;
using namespace torch::nn;

Mv3Large::Mv3Large(const int &_numOfClass) : BaseClassifier(
        Sequential(
                Conv2d(Conv2dOptions(3, 16, {3, 3})
                               .stride({2, 2})
                               .padding({1, 1})
                               .bias(false)),
                BatchNorm2d(BatchNorm2dOptions(16)),
                HSwish()),
        {
                Sequential(Mv3BneckModule(16, 16, 16, 3, 1, Mv3Activation::Relu, false),
                           Mv3BneckModule(16, 64, 24, 3, 2, Mv3Activation::Relu, false)),
                Sequential(Mv3BneckModule(24, 72, 24, 3, 1, Mv3Activation::Relu, false),
                           Mv3BneckModule(24, 72, 40, 5, 2, Mv3Activation::Relu, true)),
                Sequential(Mv3BneckModule(40, 120, 40, 5, 1, Mv3Activation::Relu, true),
                           Mv3BneckModule(40, 120, 40, 5, 1, Mv3Activation::Relu, true),
                           Mv3BneckModule(40, 240, 80, 3, 2, Mv3Activation::HSwish, false)),
                Sequential(Mv3BneckModule(80, 200, 80, 3, 1, Mv3Activation::HSwish, false),
                           Mv3BneckModule(80, 184, 80, 3, 1, Mv3Activation::HSwish, false),
                           Mv3BneckModule(80, 184, 80, 3, 1, Mv3Activation::HSwish, false),
                           Mv3BneckModule(80, 480, 112, 3, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(112, 672, 112, 3, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(112, 672, 160, 5, 2, Mv3Activation::HSwish, true)),
                Sequential(Mv3BneckModule(160, 960, 160, 5, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(160, 960, 160, 5, 1, Mv3Activation::HSwish, true))},
        Sequential(
                // part1: fc1
                Conv2d(Conv2dOptions(160, 960, {1, 1})
                               .bias(false)),
                BatchNorm2d(BatchNorm2dOptions(960)),
                HSwish(),
                // part2: apply avg pool
                AdaptiveAvgPool2d(AdaptiveAvgPool2dOptions({1, 1})),
                // part3: fc2
                Conv2d(Conv2dOptions(960, 1280, {1, 1})
                               .bias(true)),
                HSwish(),
                // part4: 到节点
                Conv2d(Conv2dOptions(1280, _numOfClass, {1, 1})
                               .bias(true)))) {
    registerModule();
}

void Mv3Large::registerModule() {
    register_module("conv2d-in", layerIn);
    for (size_t i = 0; i < bnecks.size(); i++) {
        register_module("bneck-" + std::to_string(i), bnecks[i]);
    }
    register_module("fc-layer", layerOut);
}

Tensor Mv3Large::forward(Tensor x) {
    x = layerIn->forward(x);
    for (auto &bneck:bnecks) {
        x = bneck->forward(x);
    }
    x = layerOut->forward(x);
    return log_softmax(x.squeeze(-1).squeeze(-1), 1);
}
