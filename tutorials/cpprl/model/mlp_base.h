#pragma once

#include "model/nn_base.h"

#include <torch/types.h>
#include <torch/nn/modules/container/sequential.h>
#include <torch/nn/modules/linear.h>
#include <vector>

using namespace torch;

namespace cpprl {
    class MlpBase : public NNBase {
    private:
        nn::Sequential actor;
        nn::Sequential critic;
        nn::Linear critic_linear;
        unsigned int num_inputs;

    public:
        MlpBase(unsigned int num_inputs,
                bool recurrent = false,
                unsigned int hidden_size = 64);

        std::vector<torch::Tensor> forward(torch::Tensor inputs,
                                           torch::Tensor hxs,
                                           torch::Tensor masks);

        inline unsigned int get_num_inputs() const { return num_inputs; }
    };
}