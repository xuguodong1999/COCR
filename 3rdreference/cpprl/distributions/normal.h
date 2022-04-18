#pragma once

#include "distributions/distribution.h"

#include <torch/types.h>

namespace cpprl {
    class Normal : public Distribution {
    private:
        torch::Tensor loc, scale;

    public:
        Normal(const torch::Tensor loc, const torch::Tensor scale);

        torch::Tensor entropy();

        torch::Tensor log_prob(torch::Tensor value);

        torch::Tensor sample(c10::ArrayRef<int64_t> sample_shape = {});

        inline torch::Tensor get_loc() { return loc; }

        inline torch::Tensor get_scale() { return scale; }
    };
}