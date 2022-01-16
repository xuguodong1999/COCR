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

Mv3BneckModuleImpl::Mv3BneckModuleImpl(
        const int &_inputSize, const int &_expandSize, const int &_outputSize,
        const int &_kernelSize, const int &_stride,
        const Mv3Activation &_actType, bool _withSE) {
    downSample = (_stride != 1);
    layers->push_back(Conv2d(Conv2dOpt(
            _inputSize, _expandSize, {1, 1}).bias(false)));
    layers->push_back(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(_expandSize)));
    auto addActivation = [&]() -> void {
        switch (_actType) {
            case Mv3Activation::Relu:
                layers->push_back(torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)));
                break;
            case Mv3Activation::HSwish:
                layers->push_back(HSwish());
                break;
            default:
                std::cerr << "unknown activation enum type" << std::endl;
                exit(-1);
        }
    };
    addActivation();
    layers->push_back(Conv2d(Conv2dOpt(
            _expandSize, _expandSize, {_kernelSize, _kernelSize})
                                     .stride({_stride, _stride})
                                     .padding({_kernelSize / 2, _kernelSize / 2})
                                     .groups(_expandSize)
                                     .bias(false)));
    layers->push_back(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(_expandSize)));
    addActivation();
    layers->push_back(Conv2d(Conv2dOpt(
            _expandSize, _outputSize, {1, 1}).bias(false)));
    layers->push_back(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(_outputSize)));
    if (_withSE) {
        layers->push_back(SeModule(_outputSize));
    }
    register_module("bneck", layers);
    if (_inputSize != _outputSize) {
        shortcut->push_back(Conv2d(Conv2dOpt(
                _inputSize, _outputSize, {1, 1}).bias(false)));
        shortcut->push_back(torch::nn::BatchNorm2d(
                torch::nn::BatchNorm2dOptions(_outputSize)));
        register_module("shortcut", shortcut);
    }
}

torch::Tensor Mv3BneckModuleImpl::forward(torch::Tensor input) {
    if (downSample) {
        return layers->forward(input);
    } else {
        if (shortcut->size() == 0) {
            return layers->forward(input) + input;
        } else {
            return layers->forward(input) + shortcut->forward(input);
        }
    }
}