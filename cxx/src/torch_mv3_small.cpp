#include "torch_mv3.hpp"
#include "torch_activation.hpp"
#include "torch_module.hpp"

using namespace torch;
using namespace torch::nn;

Mv3Small::Mv3Small(int _numOfClass) : BaseClassifier(
        Sequential(
                Conv2d(Conv2dOptions(3, 16, {3, 3})
                               .stride({2, 2})
                               .padding({1, 1})
                               .bias(false)),
                BatchNorm2d(BatchNorm2dOptions(16)),
                HSwish()),
        {
                Sequential(Mv3BneckModule(16, 16, 16, 3, 2, Mv3Activation::Relu, true)),
                Sequential(Mv3BneckModule(16, 72, 24, 3, 2, Mv3Activation::Relu, false)),
                Sequential(Mv3BneckModule(24, 88, 24, 3, 1, Mv3Activation::Relu, false),
                           Mv3BneckModule(24, 96, 40, 5, 2, Mv3Activation::HSwish, true)),
                Sequential(Mv3BneckModule(40, 240, 40, 5, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(40, 240, 40, 5, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(40, 120, 48, 5, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(48, 144, 48, 5, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(48, 288, 96, 5, 2, Mv3Activation::HSwish, true)),
                Sequential(Mv3BneckModule(96, 576, 96, 5, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(96, 576, 96, 5, 1, Mv3Activation::HSwish, true))},
        Sequential(
                // part1: fc1
                Conv2d(Conv2dOptions(96, 576, {1, 1})
                               .bias(false)),
                BatchNorm2d(BatchNorm2dOptions(576)),
                HSwish(),
                // part2: apply avg pool
                AdaptiveAvgPool2d(AdaptiveAvgPool2dOptions({1, 1})),
                // part3: fc2
                Conv2d(Conv2dOptions(576, 1024, {1, 1})
                               .bias(true)),
                HSwish(),
                // part4: 到节点
                Conv2d(Conv2dOptions(1024, _numOfClass, {1, 1})
                               .bias(true)))) {
    registerModule();
}

void Mv3Small::registerModule() {
    register_module("conv2d-in", layerIn);
    for (size_t i = 0; i < bnecks.size(); i++) {
        register_module("bneck-" + std::to_string(i), bnecks[i]);
    }
    register_module("fc-layer", layerOut);
}

Tensor Mv3Small::forward(Tensor x) {
    x = layerIn->forward(x);
    for (auto &bneck:bnecks) {
        x = bneck->forward(x);
    }
    x = layerOut->forward(x);
    x = x.squeeze(-1);
    x = x.squeeze(-1);
    return log_softmax(x, 1);
}
