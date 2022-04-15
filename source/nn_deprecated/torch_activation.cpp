#include "torch_activation.hpp"
#include <iostream>

using namespace torch;
using namespace std;

Tensor SwishImpl::forward(const Tensor &input) {
    return input * sigmoid(input);
}

void SwishImpl::reset() {}

void SwishImpl::pretty_print(ostream &stream) const {
    stream << "Swish()";
}

void HSigmoidImpl::reset() {}

void HSigmoidImpl::pretty_print(ostream &stream) const {
    stream << "HSigmoid()";
}

#define __relu6__(x) \
nn::functional::relu6(((x)), nn::functional::ReLU6FuncOptions(true))

Tensor HSigmoidImpl::forward(const Tensor &input) {
    return __relu6__(input + 3) / 6;
}

void HSwishImpl::reset() {}

void HSwishImpl::pretty_print(ostream &stream) const {
    stream << "HSwish()";
}

Tensor HSwishImpl::forward(const Tensor &input) {
    return input * __relu6__(input + 3) / 6;
}

Tensor ReluImpl::forward(const Tensor &input) {
    return nn::functional::relu(input, nn::functional::ReLUFuncOptions(true));
}

void ReluImpl::reset() {

}

void ReluImpl::pretty_print(ostream &stream) const {
    stream << "Relu()";
}
