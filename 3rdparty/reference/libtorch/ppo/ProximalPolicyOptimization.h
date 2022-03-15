#pragma once

#include <torch/types.h>
#include <random>
#include <torch/optim/optimizer.h>

#include "Models.h"

// Vector of tensors.
using VT = std::vector<torch::Tensor>;

// Optimizer.
using OPT = torch::optim::Optimizer;

// Random engine for shuffling memory.
static std::random_device rd;
static std::mt19937 re(rd());

// Proximal policy optimization, https://arxiv.org/abs/1707.06347
class PPO {
public:
    static VT returns(VT &rewards, VT &dones, VT &vals, double gamma,
                      double lambda) { // Generalized advantage estimate, https://arxiv.org/abs/1506.02438
        // Compute the returns.
        torch::Tensor gae = torch::zeros({1}, torch::kFloat64);
        VT returns(rewards.size(), torch::zeros({1}, torch::kFloat64));

        for (uint i = rewards.size(); i-- >
                                      0;) // inverse for loops over unsigned: https://stackoverflow.com/questions/665745/whats-the-best-way-to-do-a-reverse-for-loop-with-an-unsigned-index/665773
        {
            // Advantage.
            auto delta = rewards[i] + gamma * vals[i + 1] * (1 - dones[i]) - vals[i];
            gae = delta + gamma * lambda * (1 - dones[i]) * gae;

            returns[i] = gae + vals[i];
        }
        return returns;
    }

    static void update(ActorCritic &ac,
                       torch::Tensor &states,
                       torch::Tensor &actions,
                       torch::Tensor &log_probs,
                       torch::Tensor &returns,
                       torch::Tensor &advantages,
                       OPT &opt,
                       uint steps, uint epochs, uint mini_batch_size, double beta, double clip_param = .2) {
        for (uint e = 0; e < epochs; e++) {
            // Generate random indices.
            torch::Tensor cpy_sta = torch::zeros({mini_batch_size, states.size(1)}, states.options());
            torch::Tensor cpy_act = torch::zeros({mini_batch_size, actions.size(1)}, actions.options());
            torch::Tensor cpy_log = torch::zeros({mini_batch_size, log_probs.size(1)}, log_probs.options());
            torch::Tensor cpy_ret = torch::zeros({mini_batch_size, returns.size(1)}, returns.options());
            torch::Tensor cpy_adv = torch::zeros({mini_batch_size, advantages.size(1)}, advantages.options());

            for (uint b = 0; b < mini_batch_size; b++) {

                uint idx = std::uniform_int_distribution<uint>(0, steps - 1)(re);
                cpy_sta[b] = states[idx];
                cpy_act[b] = actions[idx];
                cpy_log[b] = log_probs[idx];
                cpy_ret[b] = returns[idx];
                cpy_adv[b] = advantages[idx];
            }

            auto av = ac->forward(cpy_sta); // action value pairs
            auto action = std::get<0>(av);
            auto entropy = ac->entropy().mean();
            auto new_log_prob = ac->log_prob(cpy_act);

            auto old_log_prob = cpy_log;
            auto ratio = (new_log_prob - old_log_prob).exp();
            auto surr1 = ratio * cpy_adv;
            auto surr2 = torch::clamp(ratio, 1. - clip_param, 1. + clip_param) * cpy_adv;

            auto val = std::get<1>(av);
            auto actor_loss = -torch::min(surr1, surr2).mean();
            auto critic_loss = (cpy_ret - val).pow(2).mean();

            auto loss = 0.5 * critic_loss + actor_loss - beta * entropy;

            opt.zero_grad();
            loss.backward();
            opt.step();
        }
    }
};
