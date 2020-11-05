#include "mv3.hpp"

Mv3Small::Mv3Small(int num_classes) :
        conv_in(torch::nn::Conv2dOptions(3, 16, {3, 3})
                        .stride({2, 2})
                        .padding({1, 1})
                        .bias(false)),
        bn_in(torch::nn::BatchNorm2dOptions(16)),
        act_in(),
        bneck(torch::nn::Sequential(
                BlockReLUModule(
                        3, 16, 16, 16,
                        torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
                        SeModule(16),
                        2),
                BlockReLUNullModule(
                        3, 16, 72, 24,
                        torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
                        2),
                BlockReLUNullModule(
                        3, 24, 88, 24,
                        torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
                        1),
                BlockHSwishModule(
                        5, 24, 96, 40,
                        HSwish(),
                        SeModule(40),
                        2),
                BlockHSwishModule(
                        5, 40, 240, 40,
                        HSwish(),
                        SeModule(40),
                        1),
                BlockHSwishModule(
                        5, 40, 240, 40,
                        HSwish(),
                        SeModule(40),
                        1),
                BlockHSwishModule(
                        5, 40, 120, 48,
                        HSwish(),
                        SeModule(48),
                        1),
                BlockHSwishModule(
                        5, 48, 144, 48,
                        HSwish(),
                        SeModule(48),
                        1),
                BlockHSwishModule(
                        5, 48, 288, 96,
                        HSwish(),
                        SeModule(96),
                        2),
                BlockHSwishModule(
                        5, 96, 576, 96,
                        HSwish(),
                        SeModule(96),
                        1),
                BlockHSwishModule(
                        5, 96, 576, 96,
                        HSwish(),
                        SeModule(96),
                        1)
        )),
        conv_bneck_out(torch::nn::Conv2dOptions(
                96, 576, {1, 1}).bias(false)),
        bn_bneck_out(torch::nn::BatchNorm2dOptions(576)),

        act_bneck_out(),
        conv_fc1(torch::nn::Conv2dOptions(
                576, 1024, {1, 1}).bias(true)),
//        bn_fc1(1024),
        act_fc1(),
        conv_fc2(torch::nn::Conv2dOptions(
                1024, num_classes, {1, 1}).bias(true)) {
    register_module("conv1", conv_in);
    register_module("conv2", conv_bneck_out);
    register_module("bn1", bn_in);
    register_module("bn2", bn_bneck_out);
//    register_module("bn3", bn_fc1);
    register_module("hs1", act_in);
    register_module("hs2", act_bneck_out);
    register_module("hs3", act_fc1);
    register_module("linear3", conv_fc1);
    register_module("linear4", conv_fc2);
    register_module("bneck", bneck);
}

torch::Tensor Mv3Small::forward(torch::Tensor x) {
    auto out = act_in->forward(bn_in->forward(conv_in->forward(x)));
    out = bneck->forward(out);
    out = act_bneck_out->forward(bn_bneck_out->forward(conv_bneck_out->forward(out)));
    out = torch::adaptive_avg_pool2d(out, {1, 1});
    // FIXME: NBN here
//    out = act_fc1->forward(bn_fc1->forward(conv_fc1->forward(out)));
    out = act_fc1->forward(conv_fc1->forward(out));
    out = conv_fc2->forward(out);
    out = out.view({out.size(0), -1});
    return torch::log_softmax(out, 1);
}

