#pragma once

#include "xgd_data_export.h"

#include <string>

class XGD_DATA_EXPORT SOSODarknet {
    inline static const std::string imgDir = "/JPEGImages/";
    inline static const std::string labelDir = "/labels/";
    bool isInited;
    std::string imgPath, labelPath;
public:
    SOSODarknet();

    bool init(const std::string &_topDir);

    void dump(const size_t &_numOfSamples, const size_t &_repeatTimes = 5);

    void display();
};