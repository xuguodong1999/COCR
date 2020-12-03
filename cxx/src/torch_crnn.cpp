#include "torch_crnn.hpp"
#include "torch_activation.hpp"
#include "torch_module.hpp"

using namespace torch;
using namespace torch::nn;

CRNN::CRNN(const int &_numOfClass, const float &_mv3Scale) : BaseClassifier(
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
