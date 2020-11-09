#include "torch_cifar_dataset.hpp"
#include <opencv2/opencv.hpp>

class Cifar10Util {
    bool isTrain;

    const uint32_t mTrainNum = 50000;

    const uint32_t mTestNum = 10000;

    const uint32_t batchWidth = 32;

    const uint32_t batchHeight = 32;

    const uint32_t mChannelBytes = batchWidth * batchHeight;

    uint32_t fileBytes() {
        return bytesPerSample() * (isTrain ? trainNumPerFile() : testNumPerFile());
    }

protected:
    virtual uint32_t trainNumPerFile() { return 10000; }

    virtual uint32_t testNumPerFile() { return 10000; }

    virtual uint32_t bytesPerSample() { return 3073; }

    virtual size_t labelOffset() { return 0; }

    virtual const std::vector<std::string> trainFiles() {
        return {
                "data_batch_1.bin",
                "data_batch_2.bin",
                "data_batch_3.bin",
                "data_batch_4.bin",
                "data_batch_5.bin",
        };
    }

    virtual const std::vector<std::string> testFiles() {
        return {"test_batch.bin"};
    }

public:
    std::pair<torch::Tensor, torch::Tensor> loadData(const std::string &root, bool train) {
        isTrain = train;
        const auto files = train ? trainFiles() : testFiles();
        const auto num_samples = train ? mTrainNum : mTestNum;

        std::vector<uchar> data_buffer;
        data_buffer.reserve(files.size() * fileBytes());

        for (const auto &file : files) {
            const auto path = root + "/" + file;
            std::ifstream data(path, std::ios::binary);
            TORCH_CHECK(data, "Error opening data file at", path);
            data_buffer.insert(data_buffer.end(), std::istreambuf_iterator<char>(data), {});
        }

        TORCH_CHECK(data_buffer.size() == files.size() * fileBytes(), "Unexpected file sizes");

        auto targets = torch::empty(num_samples, torch::kByte);
        auto images = torch::empty({num_samples, 3, batchWidth, batchHeight}, torch::kByte);

        for (uint32_t i = 0; i != num_samples; ++i) {
            // The first byte of each row is the target class index.
            uint32_t start_index = i * bytesPerSample() + labelOffset();
            targets[i] = data_buffer[start_index];

            // The next bytes correspond to the rgb channel values in the following order:
            // red (32 *32 = 1024 bytes) | green (1024 bytes) | blue (1024 bytes)
            uint32_t image_start = start_index + 1;
            uint32_t image_end = image_start + 3 * mChannelBytes;

            cv::Mat img;
            uchar *begin = data_buffer.data() + image_start;
            cv::merge(std::vector<cv::Mat>(
                    {cv::Mat(32, 32, CV_8UC1,
                             (begin + 2 * mChannelBytes)),//b
                     cv::Mat(32, 32, CV_8UC1,
                             (begin + mChannelBytes)),//g
                     cv::Mat(32, 32, CV_8UC1, (begin))//r
                    }), img);
            img.convertTo(img, CV_32F, 1.0 / 255);
            cv::imshow("", img);
            cv::waitKey(0);
            images[i] = torch::from_blob(
                    img.data, {batchWidth, batchHeight, 3}, torch::kFloat
            ).permute({2, 0, 1}).contiguous();
        }
        return {std::move(images), targets.to(torch::kInt64)};
    }
};

class Cifar100Util : public Cifar10Util {
public:
    uint32_t trainNumPerFile() override { return 50000; }

    uint32_t testNumPerFile() override { return 10000; }

    uint32_t bytesPerSample() override { return 3074; }

    const std::vector<std::string> trainFiles() override {
        return {"train.bin"};
    }

    size_t labelOffset() { return 1; }

    const std::vector<std::string> testFiles() override {
        return {"test.bin"};
    }
};

CifarDataSet::CifarDataSet(const CifarType &cifarType, const std::string &root, Mode mode) : mMode(mode) {
    std::pair<torch::Tensor, torch::Tensor> data;
    if (CifarType::CIFAR10 == cifarType) {
        Cifar10Util cfg;
        data = cfg.loadData(root, mode == Mode::kTrain);
    } else if (CifarType::CIFAR100 == cifarType) {
        Cifar100Util cfg;
        data = cfg.loadData(root, mode == Mode::kTrain);
    }
    mImages = std::move(data.first);
    mTargets = std::move(data.second);
}

torch::data::Example<> CifarDataSet::get(size_t index) {
    return {mImages[index], mTargets[index]};
}

torch::optional<size_t> CifarDataSet::size() const {
    return mImages.size(0);
}

bool CifarDataSet::isTrainMode() const noexcept {
    return mMode == Mode::kTrain;
}



