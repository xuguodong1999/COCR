#include "torch_yolov4.hpp"
#include "torch_activation.hpp"
#include "torch_module.hpp"
#include <cmath>

using namespace torch;
using namespace torch::nn;

#define RESCALE(a) std::round((a)*_mv3Scale)

Yolov4::Yolov4(const float &_mv3Scale) : BaseClassifier(
        Sequential(
                Conv2d(Conv2dOptions(3, RESCALE(16), {3, 3})
                               .stride({2, 2})
                               .padding({1, 1})
                               .bias(false)),
                BatchNorm2d(BatchNorm2dOptions(RESCALE(16))),
                HSwish()),
        {
                Sequential(Mv3BneckModule(RESCALE(16), RESCALE(16), RESCALE(16),
                                          3, 1, Mv3Activation::Relu, false),
                           Mv3BneckModule(RESCALE(16), RESCALE(64), RESCALE(24),
                                          3, 2, Mv3Activation::Relu, false)),
                Sequential(Mv3BneckModule(RESCALE(24), RESCALE(72), RESCALE(24),
                                          3, 1, Mv3Activation::Relu, false),
                           Mv3BneckModule(RESCALE(24), RESCALE(72), RESCALE(40),
                                          5, 2, Mv3Activation::Relu, true)),
                Sequential(Mv3BneckModule(RESCALE(40), RESCALE(120), RESCALE(40),
                                          5, 1, Mv3Activation::Relu, true),
                           Mv3BneckModule(RESCALE(40), RESCALE(120), RESCALE(40),
                                          5, 1, Mv3Activation::Relu, true),
                           Mv3BneckModule(RESCALE(40), RESCALE(240), RESCALE(80),
                                          3, 2, Mv3Activation::HSwish, false)),
                Sequential(Mv3BneckModule(RESCALE(80), RESCALE(200), RESCALE(80),
                                          3, 1, Mv3Activation::HSwish, false),
                           Mv3BneckModule(RESCALE(80), RESCALE(184), RESCALE(80),
                                          3, 1, Mv3Activation::HSwish, false),
                           Mv3BneckModule(RESCALE(80), RESCALE(184), RESCALE(80),
                                          3, 1, Mv3Activation::HSwish, false),
                           Mv3BneckModule(RESCALE(80), RESCALE(480), RESCALE(112),
                                          3, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(RESCALE(112), RESCALE(672), RESCALE(112),
                                          3, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(RESCALE(112), RESCALE(672), RESCALE(160),
                                          5, 2, Mv3Activation::HSwish, true)),
                Sequential(Mv3BneckModule(RESCALE(160), RESCALE(960), RESCALE(160),
                                          5, 1, Mv3Activation::HSwish, true),
                           Mv3BneckModule(RESCALE(160), RESCALE(960), RESCALE(160),
                                          5, 1, Mv3Activation::HSwish, true))},
        Sequential()) {
    setName("mv3-yolov4");
    // add detection-header
    auto &d32x = bnecks[4];
    auto &d16x = bnecks[3];
    auto &d8x = bnecks[2];
    // 定义两个2x上采样反卷积、两个2x下采样卷积、若干1x1，3x3卷积、三个检测头
}
