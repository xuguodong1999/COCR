#include "torch_yolov4.hpp"
#include "torch_activation.hpp"
#include "torch_module.hpp"

#include <torch/serialize.h>
#include <cmath>
#include <filesystem>

using namespace torch;
using namespace torch::nn;

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
        Sequential()), h32x(layerOut) {
    setName("mv3-yolov4");
    // add detection-header
    auto &f32x = bnecks[4];//160
    auto &f16x = bnecks[3];//160
    auto &f8x = bnecks[2];//80
    const std::vector<int> a = {
            RESCALE(160),// a[0]
            RESCALE(112),// a[1]
            RESCALE(40), // a[2]
            160,// a[3]
            120,// a[4]
            80,// a[5]

            80,// a[6]
            120,// a[7]
            160,// a[8]

            160,// a[9]
            120,// a[10]
            80,// a[11]

            320,// a[12]
            240,// a[13]
            160,// a[14]
    };
    prnLayerMap["conv32x_1"] = Sequential(ConvModule(a[0], a[3], 1, 1, true));
    prnLayerMap["conv16x_1"] = Sequential(ConvModule(a[1], a[4], 1, 1, true));
    prnLayerMap["conv8x_1"] = Sequential(ConvModule(a[2], a[5], 1, 1, true));
    prnLayerMap["conv_8x16x"] = Sequential(ConvModule(a[5], a[6], 3, 2, true));

    int a16x = a[4] + a[6];
    prnLayerMap["conv16x_2"] = Sequential(ConvModule(a16x, a16x / 2, 1, 1, true),
                                          ConvModule(a16x / 2, a16x, 3, 1, true),
                                          ConvModule(a16x, a[7], 1, 1, true));

    prnLayerMap["conv_16x32x"] = Sequential(ConvModule(a[7], a[8], 3, 2, true));

    int a32x = a[3] + a[8];
    prnLayerMap["conv32x_2"] = Sequential(ConvModule(a32x, a32x / 2, 1, 1, true),
                                          ConvModule(a32x / 2, a32x, 3, 1, true),
                                          ConvModule(a32x, a[9], 1, 1, true));

    prnLayerMap["conv32x_3"] = Sequential(ConvModule(a[9], a[12], 3, 1, true));
    prnLayerMap["conv32x16x"] = Sequential(UpSampleModule(a[9], a[9]));

    int b16x = a[9] + a[7];
    prnLayerMap["conv16x_3"] = Sequential(ConvModule(b16x, b16x / 2, 1, 1, true),
                                          ConvModule(b16x / 2, b16x, 3, 1, true),
                                          ConvModule(b16x, a[10], 1, 1, true));

    prnLayerMap["conv16x_4"] = Sequential(ConvModule(a[10], a[13], 3, 1, true));
    prnLayerMap["conv16x8x"] = Sequential(UpSampleModule(a[10], a[10]));

    int a8x = a[10] + a[5];
    prnLayerMap["conv8x_2"] = Sequential(ConvModule(a8x, a8x, 1, 1, true),
                                         ConvModule(a8x, a8x * 2, 3, 1, true),
                                         ConvModule(a8x * 2, a[11], 1, 1, true));
    prnLayerMap["conv8x_3"] = Sequential(ConvModule(a[11], a[14], 3, 1, true));

    h32x->push_back(YoloHeader(a[12], 3, _numOfClass, 4));
    h16x->push_back(YoloHeader(a[13], 3, _numOfClass, 4));
    h8x->push_back(YoloHeader(a[14], 3, _numOfClass, 4));
    registerModule();
}

void Yolov4::registerModule() {
    // classifier
    register_module("in", layerIn);
    for (size_t i = 0; i < bnecks.size(); i++) {
        register_module("bneck-" + std::to_string(i), bnecks[i]);
    }
    // detector
    for (auto&[name, layer]:prnLayerMap) {
        register_module(name, layer);
    }
    register_module("h32x", h32x);
    register_module("h16x", h16x);
    register_module("h8x", h8x);
}

torch::Tensor Yolov4::forward(torch::Tensor x) {
    x = layerIn->forward(x);
    x = bnecks[0]->forward(x);
    x = bnecks[1]->forward(x);
    auto f8x = bnecks[2]->forward(std::move(x));
    auto f16x = bnecks[3]->forward(f8x);
    auto f32x = bnecks[4]->forward(f16x);
    f8x = prnLayerMap["conv8x_1"]->forward(f8x);
    f16x = prnLayerMap["conv16x_1"]->forward(f16x);
    f32x = prnLayerMap["conv32x_1"]->forward(f32x);

    auto conv_8x16x = prnLayerMap["conv_8x16x"]->forward(f8x);
    f16x = cat({std::move(conv_8x16x), f16x}, 1);
    f16x = prnLayerMap["conv16x_2"]->forward(f16x);

    auto conv_16x32x = prnLayerMap["conv_16x32x"]->forward(f16x);
    f32x = cat({std::move(conv_16x32x), f32x}, 1);
    f32x = prnLayerMap["conv32x_2"]->forward(f32x);

    auto conv32x16x = prnLayerMap["conv32x16x"]->forward(f32x);
    f16x = cat({std::move(conv32x16x), f16x}, 1);
    f16x = prnLayerMap["conv16x_3"]->forward(f16x);

    auto conv16x8x = prnLayerMap["conv16x8x"]->forward(f16x);
    f8x = cat({std::move(conv16x8x), f8x}, 1);
    f8x = prnLayerMap["conv8x_2"]->forward(f8x);

    f32x = prnLayerMap["conv32x_3"]->forward(f32x);
    f32x = h32x->forward(f32x);
    f16x = prnLayerMap["conv16x_4"]->forward(f16x);
    f16x = h16x->forward(f16x);
    f8x = prnLayerMap["conv8x_3"]->forward(f8x);
    f8x = h8x->forward(f8x);
//    f8x.print();
//    f16x.print();
//    f32x.print();
    // TODO: get loss here
    return torch::Tensor();
}

torch::Tensor Yolov4::forwardNoGardWithNMS(torch::Tensor input) {
    return torch::Tensor();
}

void Yolov4::saveClassifier(const string &_saveDir, const string &_inPrefix,
                            const string &_bneckPrefix,
                            const string &_outPrefix) {
    if (!std::filesystem::exists(_saveDir)) {
        if (!std::filesystem::create_directories(_saveDir)) {
            std::cerr << "BaseClassifier::saveClassifier: Have no access to "
                      << _saveDir << std::endl;
            exit(-1);
        }
    }
    const std::string prefix = _saveDir + "/" + getName() + "-";
    try {
        torch::save(layerIn, prefix + _inPrefix + ".pth");
        for (size_t i = 0; i < bnecks.size(); i++) {
            torch::save(bnecks[i], prefix + _bneckPrefix + "-d" +
                                   std::to_string(i + 1) + "x.pth");
        }
        torch::save(h8x, prefix + _outPrefix + "-h8x.pth");
        torch::save(h16x, prefix + _outPrefix + "-h16x.pth");
        torch::save(h32x, prefix + _outPrefix + "-h32x.pth");
        for (auto&[name, layer]:prnLayerMap) {
            torch::save(layer, prefix + _outPrefix + "-" + name + ".pth");
        }
    } catch (std::exception &e) {
        std::cerr << "In Yolov4::saveClassifier" << std::endl;
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
