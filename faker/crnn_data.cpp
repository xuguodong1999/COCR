#include "crnn_data.hpp"
#include <filesystem>
#include <iostream>
#include "hw_str.hpp"
#include "linetextdata.hpp"

CRNNDataGenerator::CRNNDataGenerator() : isInited(false) {
}

bool CRNNDataGenerator::init(const char *_topDir) {
    if (!std::filesystem::exists(_topDir)) {
        if (!std::filesystem::create_directories(_topDir)) {
            std::cerr << "fail to create dir: " << _topDir << std::endl;
            exit(-1);
            return false;
        }
    }
    imgPath = _topDir + imgDir;
    labelPath = _topDir + labelDir;
    if (std::filesystem::exists(imgPath)) {
        std::filesystem::remove_all(imgPath);
    }
    if (std::filesystem::exists(labelPath)) {
        std::filesystem::remove_all(labelPath);
    }
    std::filesystem::create_directories(imgPath);
    std::filesystem::create_directories(labelPath);
    if (!std::filesystem::is_empty(imgPath) ||
        !std::filesystem::is_empty(labelPath)) {
        std::cerr << "target dir not empty: " << imgPath << " or " << labelPath << std::endl;
        exit(-1);
        return false;
    }
    isInited = true;
    return true;
}

void CRNNDataGenerator::dump(const size_t &_numOfSamples, const size_t &_repeatTimes) {
    if (!isInited) {
        std::cerr << "you must call CRNNDataGenerator::init before dump data" << std::endl;
        exit(-1);
    }
}
