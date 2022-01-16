#ifndef _DATASET_CIFAR_HPP_
#define _DATASET_CIFAR_HPP_

#include <torch/data.h>
#include <opencv2/core/mat.hpp>
#include <cstddef>
#include <fstream>
#include <string>

enum class CifarType {
    CIFAR100, CIFAR10
};

class CifarDataSet : public torch::data::datasets::Dataset<CifarDataSet> {
public:
    enum Mode {
        kTrain, kTest
    };

    explicit CifarDataSet(const CifarType &cifarType, const std::string &root, Mode mode = Mode::kTrain);

    torch::data::Example<> get(size_t index) override;

    torch::optional<size_t> size() const override;

    bool isTrainMode() const noexcept;


private:
    std::vector<cv::Mat> mImages;
    torch::Tensor mTargets;
    Mode mMode;

    inline static const uint32_t batchWidth = 32;

    inline static const uint32_t batchHeight = 32;

    class Cifar10Util {
        bool isTrain;

        const uint32_t mTrainNum = 50000;

        const uint32_t mTestNum = 10000;

        const uint32_t mChannelBytes = batchWidth * batchHeight;

        uint32_t fileBytes() {
            return bytesPerSample() * (isTrain ? trainNumPerFile() : testNumPerFile());
        }

    protected:
        virtual uint32_t trainNumPerFile();

        virtual uint32_t testNumPerFile();

        virtual uint32_t bytesPerSample();

        virtual size_t labelOffset();

        virtual const std::vector<std::string> trainFiles();

        virtual const std::vector<std::string> testFiles();

    public:
        std::pair<std::vector<cv::Mat>, torch::Tensor> loadData(
                const std::string &root, bool train);
    };

    class Cifar100Util : public Cifar10Util {
    public:
        uint32_t trainNumPerFile() override;

        uint32_t testNumPerFile() override;

        uint32_t bytesPerSample() override;

        const std::vector<std::string> trainFiles() override;

        size_t labelOffset() override;

        const std::vector<std::string> testFiles() override;
    };
};

#endif//_DATASET_CIFAR_HPP_
