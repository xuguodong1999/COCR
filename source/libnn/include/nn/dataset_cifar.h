#pragma once

#include "els_nn_export.h"

#include <torch/data.h>
#include <cstddef>
#include <fstream>
#include <string>

enum class CifarType {
    CIFAR100, CIFAR10
};

class CifarUtil;

class ELS_NN_EXPORT CifarDataSet : public torch::data::datasets::Dataset<CifarDataSet> {
public:
    enum Mode {
        kTrain, kTest
    };

    explicit CifarDataSet(const CifarType &cifarType, const std::string &root, Mode mode = Mode::kTrain);

    torch::data::Example<> get(size_t index) override;

    torch::optional<size_t> size() const override;

    bool isTrainMode() const noexcept;

private:
    std::shared_ptr<CifarUtil> p;
    torch::Tensor mTargets;
    Mode mMode;
};
