#pragma once

#include "model/nn_base.h"

#include <torch/types.h>
#include <torch/nn/modules/container/sequential.h>

#include <vector>

using namespace torch;

namespace cpprl {
    class CnnBase : public NNBase {
    private:
        nn::Sequential main;
        nn::Sequential critic_linear;

    public:
        CnnBase(unsigned int num_inputs,
                bool recurrent = false,
                unsigned int hidden_size = 512);

        std::vector<torch::Tensor> forward(torch::Tensor inputs,
                                           torch::Tensor hxs,
                                           torch::Tensor masks);
    };
}