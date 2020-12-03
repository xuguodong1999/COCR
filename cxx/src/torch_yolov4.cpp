#include "torch_yolov4.hpp"
#include "torch_activation.hpp"
#include "torch_module.hpp"
#include <cmath>

using namespace torch;
using namespace torch::nn;

#define RESCALE(a) std::lround((a)*_mv3Scale)

Yolov4::Yolov4(const int &_numOfClass, const float &_mv3Scale) : BaseClassifier(
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
        Sequential()), h32x(layerOut) {
    setName("mv3-yolov4");
    // add detection-header
    auto &f32x = bnecks[4];//160
    auto &f16x = bnecks[3];//160
    auto &f8x = bnecks[2];//80
    const std::vector<int> a = {
            160,// a[0]
            160,// a[1]
            80, // a[2]
            160,// a[3]
            160,// a[4]
            160,// a[5]
            160,// a[6]
            160,// a[7]
            160,// a[8]
            160,// a[9]
            160,// a[10]
            160,// a[11]
            160,// a[12]
            160,// a[13]
            160,// a[14]
            160,// a[15]
            160,// a[16]
    };
    prnLayerMap["conv1_32x"] = Sequential(ConvModule(a[0], 321, 1, 1, true));
    prnLayerMap["conv1_16x"] = Sequential(ConvModule(a[1], 242, 1, 1, true));
    prnLayerMap["conv1_8x"] = Sequential(ConvModule(a[2], 163, 1, 1, true));
    prnLayerMap["conv_8x_down_16x"] = Sequential(ConvModule(163, 238, 3, 2, true));

    prnLayerMap["conv2_16x"] = Sequential(ConvModule(242 + 238, 480, 1, 1, true),
                                          ConvModule(480, 960, 3, 1, true),
                                          ConvModule(960, 481, 1, 1, true));

    prnLayerMap["conv_16x_down_32x"] = Sequential(ConvModule(481, 323, 1, 1, true));

    prnLayerMap["conv2_32x"] = Sequential(ConvModule(321 + 323, 480, 1, 1, true),
                                          ConvModule(480, 640, 3, 1, true),
                                          ConvModule(640, 487, 1, 1, true));

    prnLayerMap["conv3_32x"] = Sequential(ConvModule(487, 486, 3, 1, true));
    prnLayerMap["conv32x_up_16x"] = Sequential(UpSampleModule(486, 486));

    prnLayerMap["conv3_16x"] = Sequential(ConvModule(481 + 486, 480, 1, 1, true),
                                          ConvModule(480, 960, 3, 1, true),
                                          ConvModule(960, 484, 1, 1, true));

    prnLayerMap["conv4_16x"] = Sequential(ConvModule(484, 485, 3, 1, true));
    prnLayerMap["conv16x_up_8x"] = Sequential(UpSampleModule(485, 489));

    prnLayerMap["conv2_8x"] = Sequential(ConvModule(163 + 489, 480, 1, 1, true),
                                         ConvModule(480, 540, 3, 1, true),
                                         ConvModule(540, 483, 1, 1, true));
    prnLayerMap["conv3_8x"] = Sequential(ConvModule(483, 540, 3, 1, true));

    h32x->push_back(YoloHeader(480, 3, _numOfClass, 4));
    h16x->push_back(YoloHeader(480, 3, _numOfClass, 4));
    h8x->push_back(YoloHeader(540, 3, _numOfClass, 4));

}

void Yolov4::registerModule() {
    // classifier
    register_module("conv2d-in", layerIn);
    for (size_t i = 0; i < bnecks.size(); i++) {
        register_module("bneck-" + std::to_string(i), bnecks[i]);
    }
    // detector
    for (auto&[name, layer]:prnLayerMap) {
        register_module(name, layer);
    }
    register_module("yolo-header-32x", h32x);
    register_module("yolo-header-16x", h16x);
    register_module("yolo-header-8x", h8x);
}

torch::Tensor Yolov4::forward(torch::Tensor x) {
    x = layerIn->forward(x);
    auto f8x = bnecks[2]->forward(bnecks[1]->forward(bnecks[0]->forward(x)));
    auto f16x = bnecks[3]->forward(f8x);
    auto f32x = bnecks[4]->forward(f16x);

    f16x = prnLayerMap["conv1_16x"]->forward(f16x);
    f32x = prnLayerMap["conv1_32x"]->forward(f32x);

    auto conv_8x_down_16 = prnLayerMap["conv_8x_down_16x"]->forward(f8x);
    f16x = cat({std::move(conv_8x_down_16), f16x});
    f16x = prnLayerMap["conv2_16x"]->forward(f16x);

    auto conv_16x_down_32x = prnLayerMap["conv_16x_down_32x"]->forward(f16x);
    f32x = cat({std::move(conv_16x_down_32x), f32x});
    f32x = prnLayerMap["conv2_32x"]->forward(f32x);

    auto conv32x_up_16x = prnLayerMap["conv32x_up_16x"]->forward(f32x);
    f16x = cat({std::move(conv32x_up_16x), f16x});
    f16x = prnLayerMap["conv3_16x"]->forward(f16x);

    auto conv16x_up_8x = prnLayerMap["conv16x_up_8x"]->forward(f16x);
    f8x = cat({std::move(conv16x_up_8x), f8x});
    f8x = prnLayerMap["conv2_8x"]->forward(f8x);

    f32x = prnLayerMap["conv3_32x"]->forward(f32x);
    f32x = h32x->forward(f32x);
    f16x = prnLayerMap["conv4_16x"]->forward(f16x);
    f16x = h16x->forward(f16x);
    f8x = prnLayerMap["conv3_8x"]->forward(f8x);
    f8x = h8x->forward(f8x);
    // TODO: get loss here
    return torch::Tensor();
}

torch::Tensor Yolov4::forwardNoGardWithNMS(torch::Tensor input) {
    return torch::Tensor();
}
