#include "mobilenetv3.hpp"

torch::Tensor HSwishImpl::forward(torch::Tensor x) {
    auto y = x;
    y = y + 3;
    y = torch::nn::functional::relu6(y, torch::nn::functional::ReLU6FuncOptions(true));
    y = x * y;
    y = y / 6;
    return y;
}

torch::Tensor HSigmoidImpl::forward(torch::Tensor x) {
    auto y = x;
    y = y + 3;
    y = torch::nn::functional::relu6(y, torch::nn::functional::ReLU6FuncOptions(true));
    y = y / 6;
    return y;
}

SeModuleImpl::SeModuleImpl(int in_size, int reduction) : se(
        torch::nn::AdaptiveAvgPool2d(torch::nn::AdaptiveAvgPool2dOptions({1, 1})),
//        torch::nn::Linear(torch::nn::LinearOptions(in_size,in_size/reduction)),
        torch::nn::Conv2d(torch::nn::Conv2dOptions(
                in_size, in_size / reduction, {1, 1}
        ).bias(false)),
        torch::nn::BatchNorm2d(in_size / reduction),
        torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
        torch::nn::Conv2d(torch::nn::Conv2dOptions(
                in_size / reduction, in_size, {1, 1}
        ).bias(false)),
//        torch::nn::Linear(torch::nn::LinearOptions(in_size/reduction,in_size)),
        torch::nn::BatchNorm2d(in_size),
        HSigmoid()) {
    register_module("se", se);
};

torch::Tensor SeModuleImpl::forward(torch::Tensor x) {
    auto y = se->forward(x);
    return x * y;
}

BlockHSwishModuleImpl::BlockHSwishModuleImpl(
        int kernel_size, int in_size, int expand_size, int out_size,
        HSwish nolinear, SeModule semodule, int stride) :
        stride_(stride),
        in_size_(in_size),
        out_size_(out_size),
        se(semodule),
        conv1(torch::nn::Conv2dOptions(in_size, expand_size, {1, 1})
                      .bias(false)),
        bn1(torch::nn::BatchNorm2dOptions(expand_size)),
        nolinear1(nolinear),
        conv2(torch::nn::Conv2dOptions(
                expand_size, expand_size, {kernel_size, kernel_size})
                      .stride({stride, stride})
                      .padding({kernel_size / 2, kernel_size / 2})
                      .groups(expand_size)
                      .bias(false)),
        bn2(torch::nn::BatchNorm2dOptions(expand_size)),
        nolinear2(nolinear),
        conv3(torch::nn::Conv2dOptions(expand_size, out_size, {1, 1}).bias(false)),
        bn3(torch::nn::BatchNorm2dOptions(out_size)) {
    if (stride == 1 && in_size != out_size) {
        shortcut = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(
                        in_size, out_size, {1, 1}).bias(false)),
                torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(out_size)));
    }
    register_module("conv1", conv1);
    register_module("conv2", conv2);
    register_module("conv3", conv3);
    register_module("bn1", bn1);
    register_module("bn2", bn2);
    register_module("bn3", bn3);
    register_module("nolinear1", nolinear1);
    register_module("nolinear2", nolinear2);
    register_module("se", se);
    register_module("shortcut", shortcut);

}

torch::Tensor BlockHSwishModuleImpl::forward(torch::Tensor x) {
    auto y = nolinear1->forward(bn1->forward(conv1->forward(x)));
    y = nolinear2->forward(bn2->forward(conv2->forward(y)));
    y = bn3->forward(conv3->forward(y));
    y = se->forward(y);
    if (stride_ == 1 && in_size_ != out_size_) {
        y = y + shortcut->forward(x);
    } else if (stride_ == 1 && in_size_ == out_size_) {
        y = y + x;
    }
    return y;
}

BlockHSwishNullModuleImpl::BlockHSwishNullModuleImpl(
        int kernel_size, int in_size, int expand_size, int out_size,
        HSwish nolinear, int stride) :
        stride_(stride),
        in_size_(in_size),
        out_size_(out_size),
        conv1(torch::nn::Conv2dOptions(in_size, expand_size, {1, 1})
                      .bias(false)),
        bn1(torch::nn::BatchNorm2dOptions(expand_size)),
        nolinear1(nolinear),
        conv2(torch::nn::Conv2dOptions(
                expand_size, expand_size, {kernel_size, kernel_size})
                      .stride({stride, stride})
                      .padding({kernel_size / 2, kernel_size / 2})
                      .groups(expand_size)
                      .bias(false)),
        bn2(torch::nn::BatchNorm2dOptions(expand_size)),
        nolinear2(nolinear),
        conv3(torch::nn::Conv2dOptions(
                expand_size, out_size, {1, 1}).bias(false)),
        bn3(torch::nn::BatchNorm2dOptions(out_size)) {
    if (stride == 1 && in_size != out_size) {
        shortcut = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(
                        in_size, out_size, {1, 1}).bias(false)),
                torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(out_size)));
    }
    register_module("conv1", conv1);
    register_module("conv2", conv2);
    register_module("conv3", conv3);
    register_module("bn1", bn1);
    register_module("bn2", bn2);
    register_module("bn3", bn3);
    register_module("nolinear1", nolinear1);
    register_module("nolinear2", nolinear2);
    register_module("shortcut", shortcut);
}

torch::Tensor BlockHSwishNullModuleImpl::forward(torch::Tensor x) {
    auto y = nolinear1->forward(bn1->forward(conv1->forward(x)));
    y = nolinear2->forward(bn2->forward(conv2->forward(y)));
    y = bn3->forward(conv3->forward(y));
    if (stride_ == 1 && in_size_ != out_size_) {
        y = y + shortcut->forward(x);
    } else if (stride_ == 1 && in_size_ == out_size_) {
        y = y + x;
    }
    return y;
}

BlockReLUModuleImpl::BlockReLUModuleImpl(
        int kernel_size, int in_size, int expand_size, int out_size,
        torch::nn::ReLU nolinear, SeModule semodule, int stride) :
        stride_(stride),
        in_size_(in_size),
        out_size_(out_size),
        se(semodule),
        conv1(torch::nn::Conv2dOptions(in_size, expand_size, {1, 1})
                      .bias(false)),
        bn1(torch::nn::BatchNorm2dOptions(expand_size)),
        nolinear1(nolinear),
        conv2(torch::nn::Conv2dOptions(
                expand_size, expand_size, {kernel_size, kernel_size})
                      .stride({stride, stride})
                      .padding({kernel_size / 2, kernel_size / 2})
                      .groups(expand_size)
                      .bias(false)),
        bn2(torch::nn::BatchNorm2dOptions(expand_size)),
        nolinear2(nolinear),
        conv3(torch::nn::Conv2dOptions(expand_size, out_size, {1, 1})
                      .bias(false)),
        bn3(torch::nn::BatchNorm2dOptions(out_size)) {
    if (stride == 1 && in_size != out_size) {
        shortcut = torch::nn::Sequential(
                torch::nn::Conv2d(torch::nn::Conv2dOptions(
                        in_size, out_size, {1, 1}).bias(false)),
                torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(out_size))
        );
    }

    register_module("conv1", conv1);
    register_module("conv2", conv2);
    register_module("conv3", conv3);
    register_module("bn1", bn1);
    register_module("bn2", bn2);
    register_module("bn3", bn3);
    register_module("nolinear1", nolinear1);
    register_module("nolinear2", nolinear2);
    register_module("se", se);
    register_module("shortcut", shortcut);
}

torch::Tensor BlockReLUModuleImpl::forward(torch::Tensor x) {
    auto y = nolinear1->forward(bn1->forward(conv1->forward(x)));
    y = nolinear2->forward(bn2->forward(conv2->forward(y)));
    y = bn3->forward(conv3->forward(y));
    y = se->forward(y);
    if (stride_ == 1 && in_size_ != out_size_) {
        y = y + shortcut->forward(x);
    } else if (stride_ == 1 && in_size_ == out_size_) {
        y = y + x;
    }
    return y;
}

BlockReLUNullModuleImpl::BlockReLUNullModuleImpl(
        int kernel_size, int in_size, int expand_size, int out_size,
        torch::nn::ReLU nolinear, int stride) :
        stride_(stride),
        in_size_(in_size),
        out_size_(out_size),
        conv1(torch::nn::Conv2dOptions(in_size, expand_size, {1, 1})
                      .bias(false)),
        bn1(torch::nn::BatchNorm2dOptions(expand_size)),
        nolinear1(nolinear),
        conv2(torch::nn::Conv2dOptions(
                expand_size, expand_size, {kernel_size, kernel_size})
                      .stride({stride, stride})
                      .padding({kernel_size / 2, kernel_size / 2})
                      .groups(expand_size)
                      .bias(false)),
        bn2(torch::nn::BatchNorm2dOptions(expand_size)),
        nolinear2(nolinear),
        conv3(torch::nn::Conv2dOptions(
                expand_size, out_size, {1, 1}).bias(false)),
        bn3(torch::nn::BatchNorm2dOptions(out_size)) {
    if (stride == 1 && in_size != out_size) {
        shortcut = torch::nn::Sequential(
                torch::nn::Conv2d(
                        torch::nn::Conv2dOptions(in_size, out_size, {1, 1}).bias(false)),
                torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(out_size)));
    }
    register_module("conv1", conv1);
    register_module("conv2", conv2);
    register_module("conv3", conv3);
    register_module("bn1", bn1);
    register_module("bn2", bn2);
    register_module("bn3", bn3);
    register_module("nolinear1", nolinear1);
    register_module("nolinear2", nolinear2);
    register_module("shortcut", shortcut);
}

torch::Tensor BlockReLUNullModuleImpl::forward(torch::Tensor x) {
    auto y = nolinear1->forward(bn1->forward(conv1->forward(x)));
    y = nolinear2->forward(bn2->forward(conv2->forward(y)));
    y = bn3->forward(conv3->forward(y));
    if (stride_ == 1 && in_size_ != out_size_) {
        y = y + shortcut->forward(x);
    } else if (stride_ == 1 && in_size_ == out_size_) {
        y = y + x;
    }
    return y;
}

MobileNetV3::MobileNetV3(int num_classes) :
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
                576, 1280, {1, 1}).bias(true)),
        bn_fc1(1280),
        act_fc1(),
        conv_fc2(torch::nn::Conv2dOptions(
                1280, num_classes, {1, 1}).bias(true)) {
    register_module("conv1", conv_in);
    register_module("conv2", conv_bneck_out);
    register_module("bn1", bn_in);
    register_module("bn2", bn_bneck_out);
    register_module("bn3", bn_fc1);
    register_module("hs1", act_in);
    register_module("hs2", act_bneck_out);
    register_module("hs3", act_fc1);
    register_module("linear3", conv_fc1);
    register_module("linear4", conv_fc2);
    register_module("bneck", bneck);
}

torch::Tensor MobileNetV3::forward(torch::Tensor x) {
    auto out = act_in->forward(bn_in->forward(conv_in->forward(x)));
    out = bneck->forward(out);
    out = act_bneck_out->forward(bn_bneck_out->forward(conv_bneck_out->forward(out)));
    out = torch::adaptive_avg_pool2d(out, {1, 1});
    out = act_fc1->forward(bn_fc1->forward(conv_fc1->forward(out)));
    out = conv_fc2->forward(out);
    out = out.view({out.size(0), -1});
    return torch::log_softmax(out, 1);
}
