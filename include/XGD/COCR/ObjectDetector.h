#pragma once

#include "xgd_cocr_export.h"
#include "XGD/COCR/OCRItem.h"

#include <string>
#include <memory>
#include <vector>


// FIXME: avoid crash on box with size<1
class XGD_COCR_EXPORT ObjectDetector {
protected:
    bool isInited;
    std::vector<std::string> objectNames;
    const float meanValues = 127.5, normValues = 1.0 / 127.5;
    const int sizeBase = 32;
    int maxWidth, maxHeight;

    /**
     * single channel, size aligned to sizeBase, limit to maxWidth/Height
     */
    virtual Mat preProcess(const Mat &_src);

    ObjectDetector();

public:
    virtual void freeModel() = 0;

    virtual std::pair<Mat, std::vector<DetectorObject>> detect(const Mat &_originImage) = 0;

    static std::shared_ptr<ObjectDetector> MakeInstance();
};