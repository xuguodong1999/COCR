#pragma once

#include <vector>
#include <memory>

#include <torch/nn.h>

#include "model/nn_base.h"
#include "model/output_layers.h"
#include "observation_normalizer.h"
#include "spaces.h"

using namespace torch;

namespace cpprl {
    class PolicyImpl : public nn::Module {
    private:
        ActionSpace action_space;
        std::shared_ptr<NNBase> base;
        ObservationNormalizer observation_normalizer;
        std::shared_ptr<OutputLayer> output_layer;

        std::vector<torch::Tensor> forward_gru(torch::Tensor x,
                                               torch::Tensor hxs,
                                               torch::Tensor masks);

    public:
        PolicyImpl(ActionSpace action_space,
                   std::shared_ptr<NNBase> base,
                   bool normalize_observations = false);

        std::vector<torch::Tensor> act(torch::Tensor inputs,
                                       torch::Tensor rnn_hxs,
                                       torch::Tensor masks) const;

        std::vector<torch::Tensor> evaluate_actions(torch::Tensor inputs,
                                                    torch::Tensor rnn_hxs,
                                                    torch::Tensor masks,
                                                    torch::Tensor actions) const;

        torch::Tensor get_probs(torch::Tensor inputs,
                                torch::Tensor rnn_hxs,
                                torch::Tensor masks) const;

        torch::Tensor get_values(torch::Tensor inputs,
                                 torch::Tensor rnn_hxs,
                                 torch::Tensor masks) const;

        void update_observation_normalizer(torch::Tensor observations);

        inline bool is_recurrent() const { return base->is_recurrent(); }

        inline unsigned int get_hidden_size() const {
            return base->get_hidden_size();
        }

        inline bool using_observation_normalizer() const {
            return !observation_normalizer.is_empty();
        }
    };

    TORCH_MODULE(Policy);
}