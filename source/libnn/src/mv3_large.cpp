#include "nn/mv3.h"
#include "nn/activation.h"
#include "nn/module.h"
#include <torch/torch.h>

using namespace torch;
using namespace torch::nn;

#define RESCALE(a) static_cast<int>(std::lround((a)*_mv3Scale))

Mv3Large::Mv3Large(const int &_numOfClass, const float &_mv3Scale) : BaseClassifier(
        Sequential(
                Conv2d(Conv2dOptions(3, RESCALE(16), {3, 3})
                               .stride({2, 2})
                               .padding({1, 1})
                               .bias(false)),
                BatchNorm2d(BatchNorm2dOptions(RESCALE(16))),
                HSwish()),
        {
                Sequential(Mv3BneckModule(RESCALE(16), RESCALE(16), RESCALE(16),
                                          3, 1, Mv3Activation::Relu, false)),
                Sequential(Mv3BneckModule(RESCALE(16), RESCALE(64), RESCALE(24),
                                          3, 2, Mv3Activation::Relu, false),
                           Mv3BneckModule(RESCALE(24), RESCALE(72), RESCALE(24),
                                          3, 1, Mv3Activation::Relu, false)),
                Sequential(Mv3BneckModule(RESCALE(24), RESCALE(72), RESCALE(40),
                                          5, 2, Mv3Activation::Relu, true),
                           Mv3BneckModule(RESCALE(40), RESCALE(120), RESCALE(40),
                                          5, 1, Mv3Activation::Relu, true),
                           Mv3BneckModule(RESCALE(40), RESCALE(120), RESCALE(40),
                                          5, 1, Mv3Activation::Relu, true)),
                Sequential(Mv3BneckModule(RESCALE(40), RESCALE(240), RESCALE(80),
                                          3, 2, Mv3Activation::HSwish, false),
                           Mv3BneckModule(RESCALE(80), RESCALE(200), RESCALE(80),
                                          3, 1, Mv3Activation::HSwish, false),
                           Mv3BneckModule(RESCALE(80), RESCALE(184), RESCALE(80),
                                          3, 1, Mv3Activation::HSwish, false),
                           Mv3BneckModule(RESCALE(80), RESCALE(184), RESCALE(80),
                                          3, 1, Mv3Activation::HSwish, false),
                           Mv3BneckModule(RESCALE(80), RESCALE(480), RESCALE(112),
                                          3, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(RESCALE(112), RESCALE(672), RESCALE(112),
                                          3, 1, Mv3Activation::HSwish, true)),
                Sequential(Mv3BneckModule(RESCALE(112), RESCALE(672), RESCALE(160),
                                          5, 2, Mv3Activation::HSwish, true),
                           Mv3BneckModule(RESCALE(160), RESCALE(960), RESCALE(160),
                                          5, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(RESCALE(160), RESCALE(960), RESCALE(160),
                                          5, 1, Mv3Activation::HSwish, true))},
        Sequential(
                // part1: fc1
                Conv2d(Conv2dOptions(RESCALE(160), RESCALE(960), {1, 1})
                               .bias(false)),
                BatchNorm2d(BatchNorm2dOptions(RESCALE(960))),
                HSwish(),
                // part2: apply avg pool
                AdaptiveAvgPool2d(AdaptiveAvgPool2dOptions({1, 1})),
                // part3: fc2
                Conv2d(Conv2dOptions(RESCALE(960), RESCALE(1280), {1, 1})
                               .bias(true)),
                HSwish(),
                // part4: 到节点
                Conv2d(Conv2dOptions(RESCALE(1280), _numOfClass, {1, 1})
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
    for (auto &bneck: bnecks) {
        x = bneck->forward(x);
    }
    x = layerOut->forward(x);
    return log_softmax(x.squeeze(-1).squeeze(-1), 1);
}
