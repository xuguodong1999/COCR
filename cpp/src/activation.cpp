#include "activation.hpp"
#include <torch/torch.h>

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