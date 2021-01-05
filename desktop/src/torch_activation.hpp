#ifndef _TORCH_ACTIVATION_HPP_
#define _TORCH_ACTIVATION_HPP_

#include <torch/nn.h>
#include <torch/nn/cloneable.h>

class SwishImpl : public torch::nn::Cloneable<SwishImpl> {
public:
    torch::Tensor forward(const torch::Tensor &input);

    void reset() override;

    void pretty_print(std::ostream &stream) const override;
};

TORCH_MODULE(Swish);

class HSigmoidImpl : public torch::nn::Cloneable<HSigmoidImpl> {
public:
    torch::Tensor forward(const torch::Tensor &input);

    void reset() override;

    void pretty_print(std::ostream &stream) const override;
};

TORCH_MODULE(HSigmoid);

class HSwishImpl : public torch::nn::Cloneable<HSwishImpl> {
public:
    torch::Tensor forward(const torch::Tensor &input);

    void reset() override;

    void pretty_print(std::ostream &stream) const override;
};

TORCH_MODULE(HSwish);

/**
 * torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true))
 */
class ReluImpl : public torch::nn::Cloneable<ReluImpl> {
public:
    torch::Tensor forward(const torch::Tensor &input);

    void reset() override;

    void pretty_print(std::ostream &stream) const override;
};

TORCH_MODULE(Relu);

//#include <torch/autograd.h>
//
//class MemoryEfficientSwishFuncImpl
//        : public torch::autograd::Function<MemoryEfficientSwishFuncImpl> {
//public:
//    static torch::autograd::variable_list forward(
//            torch::autograd::AutogradContext *ctx,
//            torch::Tensor &x) {
//        auto result = x * torch::sigmoid(x);
//        ctx->save_for_backward({result});
//        return {result};
//    }
//
//    static torch::autograd::variable_list backward(
//            torch::autograd::AutogradContext *ctx,
//            torch::autograd::variable_list &gard_output) {
//        auto i = ctx->saved_data[0].toTensor();
//        auto sigmoid_i = torch::sigmoid(i);
//        return {gard_output[0] * (sigmoid_i * (1 + i * (1 - sigmoid_i)))};
//    }
//};
//
//class MemoryEfficientSwishImpl : public torch::nn::Module {
//public:
//    torch::Tensor forward(torch::Tensor &x) {
//        auto vec = MemoryEfficientSwishFuncImpl::apply(x);
//        return vec[0];
//    }
//};
//
//TORCH_MODULE(MemoryEfficientSwish);

#endif//_TORCH_ACTIVATION_HPP_