#ifndef LOSS_HPP
#define LOSS_HPP

// For External Library
#include <torch/types.h>


// -----------------------------------
// class{Loss}
// -----------------------------------
class Loss {
public:
    Loss() {}

    torch::Tensor operator()(torch::Tensor &input, torch::Tensor &target);
};

#endif
