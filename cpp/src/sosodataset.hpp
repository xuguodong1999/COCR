#ifndef _SOSO_DATASET_HPP_
#define _SOSO_DATASET_HPP_

#include "config.hpp"

#include <torch/torch.h>
#include <opencv2/opencv.hpp>
#include <fstream>
#include <filesystem>

using YoloLabelType = std::vector<float>;
using YoloDataType = std::pair<std::string, std::vector<YoloLabelType>>;


// based on: https://github.com/pytorch/pytorch/blob/master/torch/csrc/api/include/torch/data/datasets/mnist.h.
class SosoDataSet : public torch::data::datasets::Dataset<SosoDataSet> {
    inline static std::string ImageDir = "JPEGImages";
    inline static std::string LabelDir = "labels";
    inline static std::string ImageSuffix = ".jpg";
    inline static std::string LabelSuffix = ".txt";
    inline static int batchWidth = 640;
    inline static int batchHeight = 416;
    std::vector<YoloDataType> trainSet, testSet;

    std::vector<YoloDataType> readYoloData(const std::string &_list_txt);

public:
    static void setTrainSize(int trainWidth, int trainHeight);

    enum Mode {
        kTrain, kTest
    };

    SosoDataSet(const std::string &_train_list, const std::string &_test_list);

    torch::data::Example<> get(size_t index) override;

    torch::optional<size_t> size() const override;

    bool isTrain() const noexcept;

private:
    Mode mode;
};

#endif//_SOSO_DATASET_HPP_