#pragma once

#include "els_nn_export.h"

#include <torch/nn/module.h>
#include <string>

class ELS_NN_EXPORT NNUtil {
public:
    /**
     * 加载三通道图像为 torch::Tensor
     * @param _imgPath 图像路径
     * @return 表示为 torch::Tensor 的三通道图像
     */
    static torch::Tensor loadImgTensor(
            const std::string &_imgPath, const int &_width = -1, const int &_height = -1);
};

