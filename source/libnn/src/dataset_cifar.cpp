#include "nn/dataset_cifar.h"
#include "base/std_util.h"
#include <opencv2/opencv.hpp>

class CifarUtil {
public:
    inline static const uint32_t batchWidth = 32;

    inline static const uint32_t batchHeight = 32;

    std::vector<cv::Mat> mImages;

    bool isTrain;

    const uint32_t mTrainNum = 50000;

    const uint32_t mTestNum = 10000;

    const uint32_t mChannelBytes = batchWidth * batchHeight;

    uint32_t fileBytes() {
        return bytesPerSample() * (isTrain ? trainNumPerFile() : testNumPerFile());
    }

protected:
    CifarUtil() : isTrain(false) {}

    virtual uint32_t trainNumPerFile() = 0;

    virtual uint32_t testNumPerFile() = 0;

    virtual uint32_t bytesPerSample() = 0;

    virtual size_t labelOffset() = 0;

    virtual const std::vector<std::string> trainFiles() = 0;

    virtual const std::vector<std::string> testFiles() = 0;

public:
    std::pair<std::vector<cv::Mat>, torch::Tensor> loadData(
            const std::string &root, bool train);
};

class Cifar10Util : public CifarUtil {
public:
    uint32_t trainNumPerFile() override;

    uint32_t testNumPerFile() override;

    uint32_t bytesPerSample() override;

    const std::vector<std::string> trainFiles() override;

    size_t labelOffset() override;

    const std::vector<std::string> testFiles() override;
};

class Cifar100Util : public CifarUtil {
public:
    uint32_t trainNumPerFile() override;

    uint32_t testNumPerFile() override;

    uint32_t bytesPerSample() override;

    const std::vector<std::string> trainFiles() override;

    size_t labelOffset() override;

    const std::vector<std::string> testFiles() override;
};

CifarDataSet::CifarDataSet(const CifarType &cifarType,
                           const std::string &root, Mode mode) : mMode(mode) {
    if (CifarType::CIFAR10 == cifarType) {
        p = std::make_shared<Cifar10Util>();
    } else if (CifarType::CIFAR100 == cifarType) {
        p = std::make_shared<Cifar100Util>();
    }
    auto[images, targets]=p->loadData(root, isTrainMode());
    p->mImages = std::move(images);
    mTargets = std::move(targets);
}

torch::data::Example<> CifarDataSet::get(size_t index) {
    cv::Mat cvImg = p->mImages[index];
    if (isTrainMode() && StdUtil::byProb(0.5)) {
        // 0 竖直 1 水平 -1 同时
        cv::flip(p->mImages[index], cvImg, 1);
    } else {
        cvImg = p->mImages[index].clone();
    }
    const int padSize = 4;
    const int padSize_2 = padSize / 2;
    if (isTrainMode() && StdUtil::byProb(0.5)) {
        cv::resize(p->mImages[index], cvImg,
                   cv::Size(cvImg.cols - padSize, cvImg.rows - padSize), 0, 0, cv::INTER_CUBIC);
        cv::copyMakeBorder(cvImg, cvImg,
                           padSize_2, padSize_2, padSize_2, padSize_2,
                           cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    }
    if (isTrainMode() && StdUtil::byProb(0.1)) {
        cv::Mat noise(cvImg.size(), CV_32FC3);
        cv::randn(noise, 0, StdUtil::belowProb(0.05));
        cvImg += noise;
        cv::normalize(cvImg, cvImg, 0.0, 1.0, cv::NORM_MINMAX, CV_32F);
    }
//    cv::imshow("", cvImg);
//    cv::waitKey(0);
//    cv::resize(cvImg, cvImg, cv::Size(128, 128), 0, 0, cv::INTER_CUBIC);
    return {torch::from_blob(
            cvImg.data, {cvImg.cols, cvImg.rows, cvImg.channels()}, torch::kFloat
    ).permute({2, 0, 1}).contiguous(), mTargets[index]};
}

torch::optional<size_t> CifarDataSet::size() const {
    return p->mImages.size();
}

bool CifarDataSet::isTrainMode() const noexcept {
    return mMode == Mode::kTrain;
}

// cifarutil
std::pair<std::vector<cv::Mat>, torch::Tensor> CifarUtil::loadData(
        const std::string &root, bool train) {
    isTrain = train;
    const auto files = train ? trainFiles() : testFiles();
    const auto num_samples = train ? mTrainNum : mTestNum;

    std::vector<cv::Mat> images;
    auto targets = torch::empty(num_samples, torch::kByte);

    std::vector<uchar> dataBuffer;
    dataBuffer.reserve(files.size() * fileBytes());

    for (const auto &file: files) {
        const auto path = root + "/" + file;
        std::ifstream data(path, std::ios::binary);
        TORCH_CHECK(data, "Error opening data file at", path);
        dataBuffer.insert(dataBuffer.end(), std::istreambuf_iterator<char>(data), {});
    }

    TORCH_CHECK(dataBuffer.size() == files.size() * fileBytes(), "Unexpected file sizes");

    for (uint32_t i = 0; i != num_samples; ++i) {
        // The first byte of each row is the target class index.
        uint32_t startIndex = i * bytesPerSample() + labelOffset();
        targets[i] = dataBuffer[startIndex];
        // The next bytes correspond to the rgb channel values in the following order:
        // red (32 *32 = 1024 bytes) | green (1024 bytes) | blue (1024 bytes)
        auto imgBegin = dataBuffer.data() + startIndex + 1;
        cv::Mat img;
        cv::merge(std::vector<cv::Mat>(
                {cv::Mat(32, 32, CV_8UC1,
                         (imgBegin + 2 * mChannelBytes)),//b
                 cv::Mat(32, 32, CV_8UC1,
                         (imgBegin + mChannelBytes)),//g
                 cv::Mat(32, 32, CV_8UC1, imgBegin)//r
                }), img);
        img.convertTo(img, CV_32F, 1.0 / 255);
        cv::resize(img, img, cv::Size(128, 128), 0, 0, cv::INTER_CUBIC);
        images.push_back(std::move(img));
    }
    return {std::move(images), targets.to(torch::kInt64)};
}

// cifar10
uint32_t Cifar10Util::trainNumPerFile() { return 10000; }

uint32_t Cifar10Util::testNumPerFile() { return 10000; }

uint32_t Cifar10Util::bytesPerSample() { return 3073; }

size_t Cifar10Util::labelOffset() { return 0; }

const std::vector<std::string> Cifar10Util::trainFiles() {
    return {
            "data_batch_1.bin",
            "data_batch_2.bin",
            "data_batch_3.bin",
            "data_batch_4.bin",
            "data_batch_5.bin",
    };
}

const std::vector<std::string> Cifar10Util::testFiles() {
    return {"test_batch.bin"};
}

// cifar100
uint32_t Cifar100Util::trainNumPerFile() { return 50000; }

uint32_t Cifar100Util::testNumPerFile() { return 10000; }

uint32_t Cifar100Util::bytesPerSample() { return 3074; }

size_t Cifar100Util::labelOffset() { return 1; }


const std::vector<std::string> Cifar100Util::trainFiles() {
    return {"train.bin"};
}

const std::vector<std::string> Cifar100Util::testFiles() {
    return {"test.bin"};
}




