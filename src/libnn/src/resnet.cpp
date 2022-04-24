#include "nn/resnet.h"

#include "residual_block.h"

ResNet::ResNet(const std::array<int64_t, 3> &layers, int64_t num_classes) :
        layer1(make_layer(16, layers[0])),
        layer2(make_layer(32, layers[1], 2)),
        layer3(make_layer(64, layers[2], 2)),
        fc(64, num_classes),
        conv{conv3x3(3, 16)},
        avg_pool{8},
        bn{16},
        in_channels(16){
    register_module("conv", conv);
    register_module("bn", bn);
    register_module("relu", relu);
    register_module("layer1", layer1);
    register_module("layer2", layer2);
    register_module("layer3", layer3);
    register_module("avg_pool", avg_pool);
    register_module("fc", fc);
}

torch::Tensor ResNet::forward(torch::Tensor x) {
    auto out = conv->forward(x);
    out = bn->forward(out);
    out = relu->forward(out);
    out = layer1->forward(out);
    out = layer2->forward(out);
    out = layer3->forward(out);
    out = avg_pool->forward(out);
    out = out.view({out.size(0), -1});

    return fc->forward(out);
}

torch::nn::Sequential ResNet::make_layer(int64_t out_channels, int64_t blocks, int64_t stride) {
    torch::nn::Sequential layers;
    torch::nn::Sequential downsample{nullptr};

    if (stride != 1 || in_channels != out_channels) {
        downsample = torch::nn::Sequential{
                conv3x3(in_channels, out_channels, stride),
                torch::nn::BatchNorm2d(out_channels)
        };
    }

    layers->push_back(ResidualBlock(in_channels, out_channels, stride, downsample));

    in_channels = out_channels;

    for (int64_t i = 1; i != blocks; ++i) {
        layers->push_back(ResidualBlock(out_channels, out_channels));
    }

    return layers;
}
