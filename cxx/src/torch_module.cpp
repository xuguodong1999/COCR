#include "torch_module.hpp"
#include "torch_activation.hpp"

#include <torch/torch.h>

using namespace torch;
using namespace torch::nn;

SeModuleImpl::SeModuleImpl(const int &_inputSize, const int &_reduction) : seLayer(
        AdaptiveAvgPool2d(AdaptiveAvgPool2dOptions({1, 1})),
        Conv2d(Conv2dOptions(_inputSize, std::floor(_inputSize / _reduction),
                             {1, 1}).bias(false)),
        ReLU(functional::ReLUFuncOptions(true)),
        Conv2d(Conv2dOptions(std::floor(_inputSize / _reduction), _inputSize,
                             {1, 1}).bias(false)),
        HSigmoid()) {
    register_module("seLayer", seLayer);
};

Tensor SeModuleImpl::forward(Tensor x) {
    return x * seLayer->forward(x);
}

Mv3BneckModuleImpl::Mv3BneckModuleImpl(
        const int &_inputSize, const int &_expandSize, const int &_outputSize,
        const int &_kernelSize, const int &_stride,
        const Mv3Activation &_actType, bool _withSE) {
    downSample = (_stride != 1);
    layers->push_back(Conv2d(Conv2dOptions(
            _inputSize, _expandSize, {1, 1}).bias(false)));
    layers->push_back(BatchNorm2d(BatchNorm2dOptions(_expandSize)));
    auto addActivation = [&]() -> void {
        switch (_actType) {
            case Mv3Activation::Relu:
                layers->push_back(ReLU(functional::ReLUFuncOptions(true)));
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
    layers->push_back(Conv2d(Conv2dOptions(
            _expandSize, _expandSize, {_kernelSize, _kernelSize})
                                     .stride({_stride, _stride})
                                     .padding({_kernelSize / 2, _kernelSize / 2})
                                     .groups(_expandSize)
                                     .bias(false)));
    layers->push_back(BatchNorm2d(BatchNorm2dOptions(_expandSize)));
    addActivation();
    layers->push_back(Conv2d(Conv2dOptions(
            _expandSize, _outputSize, {1, 1}).bias(false)));
    layers->push_back(BatchNorm2d(BatchNorm2dOptions(_outputSize)));
    if (_withSE) {
        layers->push_back(SeModule(_outputSize));
    }
    register_module("bneck", layers);
    if (_inputSize != _outputSize) {
        shortcut->push_back(Conv2d(Conv2dOptions(
                _inputSize, _outputSize, {1, 1}).bias(false)));
        shortcut->push_back(BatchNorm2d(
                BatchNorm2dOptions(_outputSize)));
        register_module("shortcut", shortcut);
    }
}

Tensor Mv3BneckModuleImpl::forward(Tensor input) {
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
