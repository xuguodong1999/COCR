#pragma once

#include "els_nn_export.h"
/**
 * https://github.com/prabhuomkar/pytorch-cpp.git
 * Copyright 2020-present pytorch-cpp Authors
 */

/**
 * 仅留做参考，！！！不要使用，不要使用，不要使用！！！
 * 已知问题：
 * 1、CPU 占用太大 6核i7 ~ 90%
 * 2、变换方式和 opencv 不一致
 * 解决方案：
 * 在数据仓库侧，把 cv::Mat 作为 meta data，get() 函数里在线转换到 torch::Tensor
 */
#include <vector>
#include <torch//data.h>

class RandomHorizontalFlip : public torch::data::transforms::TensorTransform<torch::Tensor> {
public:
    // Creates a transformation that randomly horizontally flips a tensor.
    // The parameter `p` determines the probability that a tensor is flipped (default = 0.5).
    explicit RandomHorizontalFlip(double p = 0.5);

    torch::Tensor operator()(torch::Tensor input) override;

private:
    double p_;
};

class ConstantPad : public torch::data::transforms::TensorTransform<torch::Tensor> {
public:
    // Creates a transformation that pads a tensor.
    // `padding` is expected to be a vector of size 4 whose entries correspond to the
    // padding of the sides, i.e {left, right, top, bottom}. `value` determines the value
    // for the padded pixels.
    explicit ConstantPad(const std::vector<int64_t> &padding, torch::Scalar value = 0.0);

    // Creates a transformation that pads a tensor.
    // The padding will be performed using the size `padding` for all 4 sides.
    // `value` determines the value for the padded pixels.
    explicit ConstantPad(int64_t padding, torch::Scalar value = 0.0);

    torch::Tensor operator()(torch::Tensor input) override;

private:
    std::vector<int64_t> padding_;
    torch::Scalar value_;
};

class RandomCrop : public torch::data::transforms::TensorTransform<torch::Tensor> {
public:
    // Creates a transformation that randomly crops a tensor.
    // The parameter `size` is expected to be a vector of size 2
    // and determines the output size {height, width}.
    explicit RandomCrop(const std::vector<int64_t> &size);

    torch::Tensor operator()(torch::Tensor input) override;

private:
    std::vector<int64_t> size_;
};