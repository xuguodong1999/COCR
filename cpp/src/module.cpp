#include "module.hpp"
#include "activation.hpp"

#include <torch/torch.h>

using Conv2d = torch::nn::Conv2d;
using Conv2dOpt = torch::nn::Conv2dOptions;

SeModuleImpl::SeModuleImpl(const int &_inputSize, const int &_reduction) : seLayer(
        torch::nn::AdaptiveAvgPool2d(torch::nn::AdaptiveAvgPool2dOptions({1, 1})),
        Conv2d(Conv2dOpt(_inputSize, std::floor(_inputSize / _reduction),
                         {1, 1}).bias(false)),
        torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)),
        Conv2d(Conv2dOpt(std::floor(_inputSize / _reduction), _inputSize,
                         {1, 1}).bias(false)),
        HSigmoid()) {
    register_module("seLayer", seLayer);
};

torch::Tensor SeModuleImpl::forward(torch::Tensor x) {
    return x * seLayer->forward(x);
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
//    std::cout<<"BlockHSwishModuleImpl::forward"<<std::endl;
//    x.print();
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
//    std::cout<<"BlockHSwishNullModuleImpl::forward"<<std::endl;
//    x.print();
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
//    std::cout << "BlockReLUModuleImpl::forward" << std::endl;
//    x.print();
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
//    std::cout<<"BlockReLUNullModuleImpl::forward"<<std::endl;
//    x.print();
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