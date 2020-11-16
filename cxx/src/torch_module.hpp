#ifndef _TORCH_MODULE_HPP_
#define _TORCH_MODULE_HPP_

#include <torch/nn.h>

class SeModuleImpl : public torch::nn::Module {
    torch::nn::Sequential seLayer;
public:
    SeModuleImpl(const int &_inputSize, const int &_reduction = 8);

    torch::Tensor forward(torch::Tensor x);
};


TORCH_MODULE(SeModule);

enum class Mv3Activation {
    Relu, HSwish
};

class Mv3BneckModuleImpl : public torch::nn::Module {
    bool downSample;
    torch::nn::Sequential layers, shortcut;
public:
    Mv3BneckModuleImpl(const int &_inputSize, const int &_expandSize, const int &_outputSize,
                       const int &_kernelSize, const int &_stride,
                       const Mv3Activation &_actType, bool _withSE);

    torch::Tensor forward(torch::Tensor input);
};

TORCH_MODULE(Mv3BneckModule);

class UpSampleModuleImpl: public torch::nn::Module{
public:
    UpSampleModuleImpl(const int&_inputSize,const int&_outputSize);
    torch::Tensor forward(torch::Tensor input);
};

TORCH_MODULE(UpSampleModule);

class ConvModuleImpl: public torch::nn::Module{
public:
    ConvModuleImpl(const int&_inputSize,const int&_outputSize,
                   const int &_kernelSize, const int &_stride,bool _withBN);
    torch::Tensor forward(torch::Tensor input);
};

TORCH_MODULE(ConvModule);

class YoloHeaderImpl: public torch::nn::Module{
public:
    YoloHeaderImpl(const int&_inputSize,const int&_outputSize);
    torch::Tensor forward(torch::Tensor input);
};

TORCH_MODULE(YoloHeader);

#endif //_MODULE_HPP_
