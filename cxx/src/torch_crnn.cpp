#include "torch_crnn.hpp"
#include "torch_activation.hpp"
#include "torch_module.hpp"

using namespace torch;
using namespace torch::nn;

CRNN::CRNN(const int &_numOfClass) : BaseClassifier(
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
                BidirectionalLSTM(160, 128, 128),
                BidirectionalLSTM(128, 128, _numOfClass))) {
    registerModule();
}

torch::Tensor CRNN::forward(torch::Tensor x) {
    x = layerIn->forward(x);
    for (auto &bneck:bnecks) {
        x = bneck->forward(x);
    }
//    std::cout<<"fSize="<<x.sizes()<<std::endl;
    x = x.squeeze(-1);
    x = x.permute({0, 2, 1});
//    std::cout<<"fSize="<<x.sizes()<<std::endl;
    x = layerOut->forward(x);
    return x;
}

void CRNN::registerModule() {
    register_module("conv2d-in", layerIn);
    for (size_t i = 0; i < bnecks.size(); i++) {
        register_module("bneck-" + std::to_string(i), bnecks[i]);
    }
    register_module("lstm-layer", layerOut);
}
