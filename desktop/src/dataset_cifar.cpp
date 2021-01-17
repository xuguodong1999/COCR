#include "dataset_cifar.hpp"
#include "opencv_util.hpp"
#include "std_util.hpp"
#include <opencv2/opencv.hpp>

CifarDataSet::CifarDataSet(const CifarType &cifarType,
                           const std::string &root, Mode mode) : mMode(mode) {
    std::shared_ptr<Cifar10Util> cfg;
    if (CifarType::CIFAR10 == cifarType) {
        cfg = std::make_shared<Cifar10Util>();
    } else if (CifarType::CIFAR100 == cifarType) {
        cfg = std::make_shared<Cifar100Util>();
    }
    auto[images, targets]=cfg->loadData(root, isTrainMode());
    mImages = std::move(images);
    mTargets = std::move(targets);
}

torch::data::Example<> CifarDataSet::get(size_t index) {
    cv::Mat cvImg;
    if (isTrainMode() && byProb(0.5)) {
        // 0 竖直 1 水平 -1 同时
        cv::flip(mImages[index], cvImg, 1);
    } else {
        cvImg = mImages[index].clone();
    }
    if (isTrainMode() && byProb(0.5)) {
        cv::resize(mImages[index], cvImg, cv::Size(28, 28));
        cv::copyMakeBorder(cvImg, cvImg,
                           2, 2, 2, 2,
                           cv::BORDER_CONSTANT, cv::Scalar(0, 0, 0));
    }
    if (isTrainMode() && byProb(0.1)) {
        cv::Mat noise(cvImg.size(), CV_32FC3);
        cv::randn(noise, 0, belowProb(0.05));
        cvImg += noise;
        cv::normalize(cvImg, cvImg, 0.0, 1.0, cv::NORM_MINMAX, CV_32F);
    }
//    cv::imshow("", cvImg);
//    cv::waitKey(0);
    return {torch::from_blob(
            cvImg.data, {batchWidth, batchHeight, 3}, torch::kFloat
    ).permute({2, 0, 1}).contiguous(), mTargets[index]};
}

torch::optional<size_t> CifarDataSet::size() const {
    return mImages.size();
}

bool CifarDataSet::isTrainMode() const noexcept {
    return mMode == Mode::kTrain;
}

std::pair<std::vector<cv::Mat>, torch::Tensor> CifarDataSet::Cifar10Util::loadData(
        const std::string &root, bool train) {
    isTrain = train;
    const auto files = train ? trainFiles() : testFiles();
    const auto num_samples = train ? mTrainNum : mTestNum;

    std::vector<cv::Mat> images;
    auto targets = torch::empty(num_samples, torch::kByte);

    std::vector<uchar> dataBuffer;
    dataBuffer.reserve(files.size() * fileBytes());

    for (const auto &file : files) {
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
        images.push_back(std::move(img));
    }
    return {std::move(images), targets.to(torch::kInt64)};
}

uint32_t CifarDataSet::Cifar10Util::trainNumPerFile() { return 10000; }

uint32_t CifarDataSet::Cifar100Util::trainNumPerFile() { return 50000; }

uint32_t CifarDataSet::Cifar10Util::testNumPerFile() { return 10000; }

uint32_t CifarDataSet::Cifar100Util::testNumPerFile() { return 10000; }

uint32_t CifarDataSet::Cifar10Util::bytesPerSample() { return 3073; }

uint32_t CifarDataSet::Cifar100Util::bytesPerSample() { return 3074; }

size_t CifarDataSet::Cifar10Util::labelOffset() { return 0; }

size_t CifarDataSet::Cifar100Util::labelOffset() { return 1; }

const std::vector<std::string> CifarDataSet::Cifar10Util::trainFiles() {
    return {
            "data_batch_1.bin",
            "data_batch_2.bin",
            "data_batch_3.bin",
            "data_batch_4.bin",
            "data_batch_5.bin",
    };
}

const std::vector<std::string> CifarDataSet::Cifar10Util::testFiles() {
    return {"test_batch.bin"};
}


const std::vector<std::string> CifarDataSet::Cifar100Util::trainFiles() {
    return {"train.bin"};
}

const std::vector<std::string> CifarDataSet::Cifar100Util::testFiles() {
    return {"test.bin"};
}




