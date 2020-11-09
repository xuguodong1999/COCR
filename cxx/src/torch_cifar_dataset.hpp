#ifndef _TORCH_CIFAR_DATASET_HPP_
#define _TORCH_CIFAR_DATASET_HPP_
#include <torch/data/datasets/base.h>
#include <torch/data/example.h>
#include <torch/types.h>

#include <cstddef>
#include <fstream>
#include <string>
enum class CifarType{
    CIFAR100,CIFAR10
};
class CifarDataSet : public torch::data::datasets::Dataset<CifarDataSet> {
public:
    enum Mode { kTrain, kTest };

    explicit CifarDataSet(const CifarType&cifarType,const std::string& root, Mode mode = Mode::kTrain);

    torch::data::Example<> get(size_t index) override;

    torch::optional<size_t> size() const override;

    bool isTrainMode() const noexcept;

    const torch::Tensor& images() const;

    const torch::Tensor& targets() const;

private:
    torch::Tensor mImages;
    torch::Tensor mTargets;
    Mode mMode;
};
#endif//_TORCH_CIFAR_DATASET_HPP_
