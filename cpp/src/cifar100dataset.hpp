#ifndef _CIFAR100_DATASET_HPP_
#define _CIFAR100_DATASET_HPP_

#include "config.hpp"
#include <torch/data/datasets.h>
#include <unordered_map>

/**
 * 加载 cifar100 c binary 数据
 * 可以用一个实例完成 train or test mode
 */
class Cifar100DataSet : public torch::data::datasets::Dataset<Cifar100DataSet> {
    inline static std::unordered_map<int, int> sCastTable;
    inline static std::unordered_map<int, std::string> cifar100Table;
    inline static int batchWidth = 32;
    inline static int batchHeight = 32;

public:
    static size_t GetNumOfClass();

    enum Mode {
        kTrain, kTest
    };

    Cifar100DataSet(const Mode &_mode = kTrain);

    torch::data::Example<> get(size_t index) override;

    torch::optional<size_t> size() const override;

    bool isTrainMode() const noexcept;

    void setMode(bool _isTrain) noexcept;

private:
    Mode mode;
};

#endif//_CIFAR100_DATASET_HPP_