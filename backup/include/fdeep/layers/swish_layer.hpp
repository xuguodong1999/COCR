// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "fdeep/layers/activation_layer.hpp"

#include <algorithm>
#include <string>

namespace fdeep { namespace internal
{

class swish_layer : public activation_layer
{
public:
    explicit swish_layer(const std::string& name)
        : activation_layer(name)
    {
    }
protected:
    tensor transform_input(const tensor& in_vol) const override
    {
        auto activation_function = [&](float_type x) -> float_type
        {
            return x / (1. + std::exp(-x));
        };
        return transform_tensor(activation_function, in_vol);
    }
};

} } // namespace fdeep, namespace internal
