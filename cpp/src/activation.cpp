#include "activation.hpp"
#include <torch/torch.h>

torch::Tensor SwishImpl::forward(const torch::Tensor &input) {
    return input * torch::sigmoid(input);
}

void SwishImpl::reset() {}

void SwishImpl::pretty_print(std::ostream &stream) const {
    stream << "Swish()";
}

void HSigmoidImpl::reset() {}

void HSigmoidImpl::pretty_print(std::ostream &stream) const {
    stream << "HSigmoid()";
}

#define __relu6__(x) \
torch::nn::functional::relu6(((x)), torch::nn::functional::ReLU6FuncOptions(true))

torch::Tensor HSigmoidImpl::forward(const torch::Tensor &input) {
    return __relu6__(input + 3) / 6;
}

void HSwishImpl::reset() {}

void HSwishImpl::pretty_print(std::ostream &stream) const {
    stream << "HSwish()";
}

torch::Tensor HSwishImpl::forward(const torch::Tensor &input) {
    return input * __relu6__(input + 3) / 6;
}


//ReluImpl::ReluImpl() : torch::nn::ReLU(torch::nn::functional::ReLUFuncOptions(true)) {
//
//}
torch::Tensor ReluImpl::forward(const torch::Tensor &input) {
    return torch::nn::functional::relu(input, torch::nn::functional::ReLUFuncOptions(true));
}

void ReluImpl::reset() {

}

void ReluImpl::pretty_print(std::ostream &stream) const {
    stream << "Relu()";
}
