#pragma once

#include "xgd_cocr_export.h"
#include "XGD/Vision/Mat.h"
#include <vector>
#include <string>
#include <memory>

class XGD_COCR_EXPORT TextRecognizer {
protected:
    std::vector<std::string> wordVec;
    const float meanValues = 127.5, normValues = 1.0 / 127.5;
    const int dstHeight = 32;

    virtual Mat preProcess(const Mat &_src);

public:
    virtual void freeModel() = 0;

    virtual std::pair<std::string, std::vector<float>> recognize(const Mat &_originImage) = 0;

    static std::shared_ptr<TextRecognizer> MakeInstance();
};