#include "couchdataset.hpp"
#include "statistic.hpp"
#include <fstream>
#include <filesystem>
#include <random>
#include <opencv2/opencv.hpp>

void CouchDataSet::InitMap(const std::string &_allClass, const std::string &_targetClass) {
    int labelIndex;
    std::string labelContent;
    std::map<std::string, int> sMainTable;
    std::map<int, std::string> sTargetTable;
    std::ifstream ifsm(_allClass);

    if (!ifsm.is_open()) {
        std::cerr << "fail to open " << _allClass << std::endl;
        exit(-1);
    }
    while (ifsm >> labelContent >> labelIndex) {
        if (labelContent.empty()) {
            continue;
        }
        auto[_, success] = sMainTable.insert(std::make_pair(labelContent, labelIndex));
        if (!success) {
            std::cerr << "found " << labelContent << " twice. invalid cast table" << std::endl;
            exit(-1);
        }
    }
    ifsm.close();
    ifsm.open(_targetClass);
    if (!ifsm.is_open()) {
        std::cerr << "fail to open " << _targetClass << std::endl;
        exit(-1);
    }
    while (ifsm >> labelContent >> labelIndex) {
        if (labelContent.empty()) {
            continue;
        }
        auto it = sMainTable.find(labelContent);
        if (it == sMainTable.end()) {
            std::cerr << labelContent << " not found in main table" << std::endl;
            exit(-1);
        }
        sTargetTable[it->second] = it->first;
    }
    ifsm.close();
    sGlobal2GridVec.clear();
    for (auto&[index, content]:sTargetTable) {
        auto &dat = CouchLoader::GetData();
        size_t sampleNum = dat[index].size();
        for (size_t i = 0; i < sampleNum; i++) {
            sGlobal2GridVec.push_back(std::make_pair(index, i));
        }
    }
    std::shuffle(sGlobal2GridVec.begin(), sGlobal2GridVec.end(),
                 std::default_random_engine());
    sGlobal2GridVec.shrink_to_fit();
    size_t gt = 0;
    for (auto &[index, _]:sTargetTable) {
        sCastTable[index] = gt++;
    }
    sCastTable[-1] = gt++;// 全1
    sCastTable[-2] = gt++;// 全0
}

CouchDataSet::CouchDataSet(const std::string &_allClass, const std::string &_targetClass,
                           const Mode &_mode) : mode(_mode) {
    if (!isConfigLoaded) {
        CouchLoader::LoadCouchDataSet();
        InitMap(_allClass, _targetClass);
        isConfigLoaded = true;
    }
    size_t ratio = sGlobal2GridVec.size() * trainDivRatio;
    if (kTrain == mode) {
        dataset.resize(ratio);
        std::copy(sGlobal2GridVec.begin(), sGlobal2GridVec.begin() + ratio, dataset.begin());
    } else {
        dataset.resize(sGlobal2GridVec.size() - ratio);
        std::copy(sGlobal2GridVec.begin() + ratio, sGlobal2GridVec.end(), dataset.begin());
    }
    std::cout << "CouchDataSet::CouchDataSet,size=" << size().value() << std::endl;
}

void CouchDataSet::setBatchImageSize(int trainWidth, int trainHeight) {
    batchWidth = trainWidth;
    batchHeight = trainHeight;
}

torch::data::Example<> CouchDataSet::get(size_t index) {
    const int max_prob = GetNumOfClass();
    int prob = (rand() % max_prob);
    if (prob == 1) {
        return {torch::ones({3,batchWidth, batchHeight}, torch::kFloat),
                torch::tensor(sCastTable[-1]).to(torch::kInt64)};
    } else if (prob == 2) {
        return {torch::zeros({3,batchWidth, batchHeight}, torch::kFloat),
                torch::tensor(sCastTable[-2]).to(torch::kInt64)};
    }
    auto&[a, b]=dataset.at(index);
    auto item = CouchLoader::GetShapeItem(a, b);
    auto bgColor = cvWhite, strokeColor = cvBlack;
    if (mode == kTrain) {
        if ((prob % 100) / 100.0 < RC::revertColorProb) {// 反转颜色
            std::swap(strokeColor, bgColor);
        }
    }
    RC::shapeAttr.color = strokeColor;
    cv::Mat img = cv::Mat(batchHeight, batchWidth, CV_8UC3, bgColor);
    item.rotate(15 - rand() % 30);
    item.moveCenterTo(cv::Point2f(batchWidth / 2, batchHeight / 2));
    float kx = 1.0 - (std::rand() % 40) / 100.0,
            ky = 1.0 - (std::rand() % 40) / 100.0;
    int offsetx = (1.0 - kx) / 2 * batchWidth, offsety = (1.0 - ky) / 2 * batchHeight;
    if (offsetx != 0 && offsety != 0) {
        offsetx = offsetx - rand() % (2 * offsetx);
        offsety = offsety - rand() % (2 * offsety);
        item.move(cv::Point2f(offsetx, offsety));
    }
    item.resizeTo(batchWidth * kx, batchHeight * ky, true);
    item.paintTo(img);
    img.convertTo(img, CV_32F, 1.0 / 255);
    if (prob < max_prob / 2 && mode == kTrain) {
        cv::Mat noise(img.size(), CV_32FC3);
        cv::randn(noise, RC::getGaussianNoiseMean(), RC::getGaussianNoiseStddev());
        img = img + noise;
        cv::normalize(img, img, 0.0, 1.0, cv::NORM_MINMAX, CV_32F);
    }
//        std::cout<<"isTrainMode()="<<isTrainMode()<<std::endl;
//        cv::imshow("CouchDataSet::get", img);
//        cv::waitKey(0);
    auto imgTensor = torch::from_blob(
            img.data, {batchWidth, batchHeight, 3}, torch::kFloat
    ).permute({2, 0, 1}).contiguous();
    auto target = torch::tensor(sCastTable[item.mLabel]).to(torch::kInt64);
    return {std::move(imgTensor), std::move(target)};
}

torch::optional<size_t> CouchDataSet::size() const {
    return dataset.size();
}

bool CouchDataSet::isTrainMode() const noexcept {
    return mode == kTrain;
}

void CouchDataSet::shuffle() {
    std::shuffle(dataset.begin(), dataset.end(), std::default_random_engine());
}

size_t CouchDataSet::GetNumOfClass() {
    if (!isConfigLoaded) {
        std::cerr << "you must construct a CouchDataSet before get class num" << std::endl;
        exit(-1);
    }
    return sCastTable.size();
}
