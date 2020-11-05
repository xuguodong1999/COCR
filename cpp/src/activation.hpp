#ifndef _ACTIVATION_HPP_
#define _ACTIVATION_HPP_

#include <torch/autograd.h>
#include <torch/nn/module.h>

class MemoryEfficientSwishFuncImpl : public torch::autograd::Function<MemoryEfficientSwishFuncImpl> {
public:
    static torch::autograd::variable_list forward(
            torch::autograd::AutogradContext *ctx,
            torch::Tensor &x) {
        auto result = x * torch::sigmoid(x);
        ctx->save_for_backward({result});
        return {result};
    }

    static torch::autograd::variable_list backward(
            torch::autograd::AutogradContext *ctx,
            torch::autograd::variable_list &gard_output) {
        auto i = ctx->saved_data[0].toTensor();
        auto sigmoid_i = torch::sigmoid(i);
        return {gard_output[0] * (sigmoid_i * (1 + i * (1 - sigmoid_i)))};
    }
};

class MemoryEfficientSwishImpl : public torch::nn::Module {
public:
    torch::Tensor forward(torch::Tensor &x) {
        auto vec = MemoryEfficientSwishFuncImpl::apply(x);
        return vec[0];
    }
};

TORCH_MODULE(MemoryEfficientSwish);

class HSwishImpl : public torch::nn::Module {
public:
    HSwishImpl() = default;

    torch::Tensor forward(torch::Tensor x);

};

class HSwish : public torch::nn::ModuleHolder<HSwishImpl> {
public:
    using torch::nn::ModuleHolder<HSwishImpl>::ModuleHolder;
};
// TORCH_MODULE(HSwish);

class HSigmoidImpl : public torch::nn::Module {
public:
    HSigmoidImpl() = default;

    torch::Tensor forward(torch::Tensor x);
};


TORCH_MODULE(HSigmoid);

#endif//_ACTIVATION_HPP_