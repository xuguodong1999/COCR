#ifndef _MODULE_HPP_
#define _MODULE_HPP_

#include "config.hpp"
#include "activation.hpp"
#include <torch/nn.h>

class SeModuleImpl : public torch::nn::Module {
    torch::nn::Sequential seLayer;
public:
    SeModuleImpl(const int& _inputSize, const int& _reduction = 8);

    torch::Tensor forward(torch::Tensor x);
};


TORCH_MODULE(SeModule);

// 零开销表达变化维度：
// 1、HS/RELU激活函数，借助模板
// 2、步长->决定要不要拼接，初始化时添加
// 3、进出大小->决定要不要多做一次卷积，借助多态
// 4、有没有SE模块，初始化时添加

enum class Mv3Activation{
    Relu,HSwish
};
class Mv3BneckModuleImpl : public torch::nn::Module {
    bool downSample;
    torch::nn::Sequential layers,shortcut;
public:
    Mv3BneckModuleImpl(const int&_inputSize,const int&_expandSize,const int&_outputSize,
                   const int&_kernelSize,const int&_stride,
                   const Mv3Activation&_actType,bool _withSE){
        downSample = (_stride!=1);
        layers->push_back(torch::nn::Conv2d(torch::nn::Conv2dOptions(
                _inputSize, _expandSize, {1, 1}) .bias(false)));
        layers->push_back(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(_expandSize)));
        auto addActivation=[&]()->void{
            switch (_actType) {
                case Mv3Activation::Relu:
                    layers->push_back(torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)));
                    break;
                case Mv3Activation::HSwish:
                    layers->push_back(HSwish());
                    break;
                default:
                    std::cerr<<"unknown activation enum type"<<std::endl;
                    exit(-1);
            }
        };
        addActivation();
        layers->push_back(torch::nn::Conv2d(torch::nn::Conv2dOptions(
                _expandSize, _expandSize, {_kernelSize, _kernelSize})
                                                    .stride({_stride, _stride})
                                                    .padding({_kernelSize / 2, _kernelSize / 2})
                                                    .groups(_expandSize)
                                                    .bias(false)));
        layers->push_back(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(_expandSize)));
        addActivation();
        layers->push_back(torch::nn::Conv2d(torch::nn::Conv2dOptions(
                _expandSize, _outputSize, {1, 1}).bias(false)));
        layers->push_back(torch::nn::BatchNorm2d(torch::nn::BatchNorm2dOptions(_outputSize)));
        if(_withSE){
            layers->push_back(SeModule(_outputSize));
        }
        register_module("bneck", layers);
        if(_inputSize!=_outputSize){
            shortcut->push_back(torch::nn::Conv2d(torch::nn::Conv2dOptions(
                    _inputSize, _outputSize, {1, 1}).bias(false)));
            shortcut->push_back(torch::nn::BatchNorm2d(
                            torch::nn::BatchNorm2dOptions(_outputSize)));
            register_module("shortcut", shortcut);
        }
    }
    torch::Tensor forward(torch::Tensor input){
        if(downSample){
            return layers->forward(input);
        }else{
            if(shortcut->size()==0){
                return layers->forward(input) + input;
            }else{
                return layers->forward(input) +shortcut->forward(input);
            }
        }
    }
};

TORCH_MODULE(Mv3BneckModule);

class BlockHSwishModuleImpl : public torch::nn::Module {
    int stride_;
    int in_size_;
    int out_size_;
    torch::nn::Conv2d conv1;
    torch::nn::Conv2d conv2;
    torch::nn::Conv2d conv3;
    HSwish nolinear1;
    HSwish nolinear2;
    torch::nn::BatchNorm2d bn1;
    torch::nn::BatchNorm2d bn2;
    torch::nn::BatchNorm2d bn3;
    SeModule se;
    torch::nn::Sequential shortcut;
public:
    BlockHSwishModuleImpl(int kernel_size, int in_size, int expand_size, int out_size,
                          HSwish nolinear, SeModule semodule, int stride);

    torch::Tensor forward(torch::Tensor x);
};

TORCH_MODULE(BlockHSwishModule);

class BlockHSwishNullModuleImpl : public torch::nn::Module {
    int stride_;
    int in_size_;
    int out_size_;
    torch::nn::Conv2d conv1;
    torch::nn::Conv2d conv2;
    torch::nn::Conv2d conv3;
    HSwish nolinear1;
    HSwish nolinear2;
    torch::nn::BatchNorm2d bn1;
    torch::nn::BatchNorm2d bn2;
    torch::nn::BatchNorm2d bn3;

    torch::nn::Sequential shortcut;
public:
    BlockHSwishNullModuleImpl(int kernel_size, int in_size, int expand_size, int out_size,
                              HSwish nolinear, int stride);

    torch::Tensor forward(torch::Tensor x);
};

TORCH_MODULE(BlockHSwishNullModule);

class BlockReLUModuleImpl : public torch::nn::Module {
    int stride_;
    int in_size_;
    int out_size_;
    torch::nn::Conv2d conv1;
    torch::nn::Conv2d conv2;
    torch::nn::Conv2d conv3;
    torch::nn::ReLU nolinear1;
    torch::nn::ReLU nolinear2;
    torch::nn::BatchNorm2d bn1;
    torch::nn::BatchNorm2d bn2;
    torch::nn::BatchNorm2d bn3;
    SeModule se;
    torch::nn::Sequential shortcut;
public:
    BlockReLUModuleImpl(int kernel_size, int in_size, int expand_size, int out_size,
                        torch::nn::ReLU nolinear, SeModule semodule, int stride);

    torch::Tensor forward(torch::Tensor x);

};

TORCH_MODULE(BlockReLUModule);

class BlockReLUNullModuleImpl : public torch::nn::Module {
    int stride_;
    int in_size_;
    int out_size_;
    torch::nn::Conv2d conv1;
    torch::nn::Conv2d conv2;
    torch::nn::Conv2d conv3;
    torch::nn::ReLU nolinear1;
    torch::nn::ReLU nolinear2;
    torch::nn::BatchNorm2d bn1;
    torch::nn::BatchNorm2d bn2;
    torch::nn::BatchNorm2d bn3;
    torch::nn::Sequential shortcut;
public:
    BlockReLUNullModuleImpl(int kernel_size, int in_size, int expand_size, int out_size,
                            torch::nn::ReLU nolinear, int stride);

    torch::Tensor forward(torch::Tensor x);

};

TORCH_MODULE(BlockReLUNullModule);

#endif //_MODULE_HPP_
