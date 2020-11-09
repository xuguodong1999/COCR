#include "torch_cifar_dataset.hpp"

class Cifar10Util {
    bool isTrain;

    uint32_t kTrainSize() { return 50000; }

    uint32_t kTestSize() { return 10000; }

    uint32_t kImageRows() { return 32; }

    uint32_t kImageColumns() { return 32; }


    uint32_t kBytesPerChannelPerRow() { return 1024; }

    uint32_t kBytesPerBatchFile() {
        return kBytesPerRow() * (isTrain ? kSizePerTrainBatch() : kSizePerTestBatch());
    }

protected:
    virtual uint32_t kSizePerTrainBatch() { return 10000; }

    virtual uint32_t kSizePerTestBatch() { return 10000; }

    virtual uint32_t kBytesPerRow() { return 3073; }

    virtual size_t labelOffset() { return 0; }

    virtual const std::vector<std::string> kTrainDataBatchFiles() {
        return {
                "data_batch_1.bin",
                "data_batch_2.bin",
                "data_batch_3.bin",
                "data_batch_4.bin",
                "data_batch_5.bin",
        };
    }

    virtual const std::vector<std::string> kTestDataBatchFiles() {
        return {"test_batch.bin"};
    }

public:
    std::pair<torch::Tensor, torch::Tensor> read_data(const std::string &root, bool train) {
        isTrain = train;
        const auto files = train ? kTrainDataBatchFiles() : kTestDataBatchFiles();
        const auto num_samples = train ? kTrainSize() : kTestSize();

        std::vector<char> data_buffer;
        data_buffer.reserve(files.size() * kBytesPerBatchFile());

        for (const auto &file : files) {
            const auto path = root + "/" + file;
            std::ifstream data(path, std::ios::binary);
            TORCH_CHECK(data, "Error opening data file at", path);
            data_buffer.insert(data_buffer.end(), std::istreambuf_iterator<char>(data), {});
        }

        TORCH_CHECK(data_buffer.size() == files.size() * kBytesPerBatchFile(), "Unexpected file sizes");

        auto targets = torch::empty(num_samples, torch::kByte);
        auto images = torch::empty({num_samples, 3, kImageRows(), kImageColumns()}, torch::kByte);

        for (uint32_t i = 0; i != num_samples; ++i) {
            // The first byte of each row is the target class index.
            uint32_t start_index = i * kBytesPerRow() + labelOffset();
            targets[i] = data_buffer[start_index];

            // The next bytes correspond to the rgb channel values in the following order:
            // red (32 *32 = 1024 bytes) | green (1024 bytes) | blue (1024 bytes)
            uint32_t image_start = start_index + 1;
            uint32_t image_end = image_start + 3 * kBytesPerChannelPerRow();
            std::copy(data_buffer.begin() + image_start, data_buffer.begin() + image_end,
                      reinterpret_cast<char *>(images[i].data_ptr()));
        }
        return {images.to(torch::kFloat32).div_(255), targets.to(torch::kInt64)};
    }
};

class Cifar100Util : public Cifar10Util {
public:
    uint32_t kSizePerTrainBatch() override { return 50000; }

    uint32_t kSizePerTestBatch() override { return 10000; }

    uint32_t kBytesPerRow() override { return 3074; }

    const std::vector<std::string> kTrainDataBatchFiles() override {
        return {"train.bin"};
    }

    size_t labelOffset() { return 1; }

    const std::vector<std::string> kTestDataBatchFiles() override {
        return {"test.bin"};
    }
};

CifarDataSet::CifarDataSet(const CifarType &cifarType, const std::string &root, Mode mode) : mMode(mode) {
    std::pair<torch::Tensor, torch::Tensor> data;
    if (CifarType::CIFAR10 == cifarType) {
        Cifar10Util cfg;
        data = cfg.read_data(root, mode == Mode::kTrain);
    } else if (CifarType::CIFAR100 == cifarType) {
        Cifar100Util cfg;
        data = cfg.read_data(root, mode == Mode::kTrain);
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

const torch::Tensor &CifarDataSet::images() const {
    return mImages;
}

const torch::Tensor &CifarDataSet::targets() const {
    return mTargets;
}
