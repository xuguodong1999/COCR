#ifndef _TORCH_UTIL_HPP_
#define _TORCH_UTIL_HPP_

#include <torch/nn/module.h>
#include <string>

/**
 * 加载三通道图像为 torch::Tensor
 * @param _imgPath 图像路径
 * @return 表示为 torch::Tensor 的三通道图像
 */
torch::Tensor loadImgTensor(const std::string &_imgPath,
                            const int &_width = -1, const int &_height = -1);

#endif//_TORCH_UTIL_HPP_
