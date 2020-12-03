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

UpSampleModuleImpl::UpSampleModuleImpl(const int &_inputSize, const int &_outputSize) : layers(
        ConvTranspose2d(ConvTranspose2dOptions(
                _inputSize, _outputSize, {3, 3})
                                .stride(2)
                                .padding({1, 1})
                                .output_padding(1)
                                .bias(false)),
        BatchNorm2d(BatchNorm2dOptions(_outputSize)),
        LeakyReLU()) {
    register_module("deconv2d", layers);
}

torch::Tensor UpSampleModuleImpl::forward(torch::Tensor input) {
    return layers->forward(input);
}

ConvModuleImpl::ConvModuleImpl(
        const int &_inputSize, const int &_outputSize,
        const int &_kernelSize, const int &_stride, bool _withBN) : layers(
        Conv2d(Conv2dOptions(
                _inputSize, _outputSize, {_kernelSize, _kernelSize})
                       .padding({_kernelSize / 2, _kernelSize / 2})
                       .stride(1)
                       .bias(false)),
        BatchNorm2d(BatchNorm2dOptions(_outputSize)),
        LeakyReLU()) {
    register_module("conv2d", layers);
}

torch::Tensor ConvModuleImpl::forward(torch::Tensor input) {
    return layers->forward(input);
}


torch::Tensor YoloHeaderImpl::forward(torch::Tensor input) {
    return layers->forward(input);
}

YoloHeaderImpl::YoloHeaderImpl(const int &_inputSize, const int &_anchorNum,
                               const int &_numOfClass, const int &_locParmNum) : layers(
        Conv2d(Conv2dOptions(_inputSize,
                             _anchorNum * (1 + _locParmNum + _numOfClass),
                             {1, 1})
                       .stride({1, 1})
                       .bias(true))) {
    register_module("yolo-head", layers);
}

BidirectionalLSTMImpl::BidirectionalLSTMImpl(const int &_inputSize, const int &_hiddenSize, const int &_outputSize) :
        rnnLayer(LSTMOptions(_inputSize, _hiddenSize).bidirectional(true)),
        embeddingLayer(LinearOptions(2 * _hiddenSize, _outputSize)) {
    register_module("rnnLayer", rnnLayer);
    register_module("embeddingLayer", embeddingLayer);
}

torch::Tensor BidirectionalLSTMImpl::forward(torch::Tensor input) {
    auto[rnnOut, _] = rnnLayer->forward(input);
//    std::cout<<"rnnOut.size="<<rnnOut.sizes()<<std::endl;
    return embeddingLayer->forward(rnnOut);
//    std::cout<<"bLSTM.outSize="<<input.sizes()<<std::endl;
//    return input;
}
