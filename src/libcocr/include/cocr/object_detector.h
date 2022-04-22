#pragma once

#include "els_cocr_export.h"

#include "cocr/ocr_item.h"

#include <string>
#include <memory>
#include <vector>


// FIXME: 添加对宽度小于1的边框的优化
class ELS_COCR_EXPORT ObjectDetector {
protected:
    bool isInited;
    std::vector<std::string> objectNames;
    const float meanValues = 127.5, normValues = 1.0 / 127.5;
    const int sizeBase = 32;
    int maxWidth, maxHeight;

    /**
     * 默认行为：转单通道，边长向上转 sizeBase 的倍数，边长限制到 [maxWidth,maxHeight]
     * @param _src
     * @return 可以送进网络的图像
     */
    virtual Mat preProcess(const Mat &_src);

    ObjectDetector();

public:
    virtual void freeModel() = 0;

    virtual std::pair<Mat, std::vector<DetectorObject>> detect(const Mat &_originImage) = 0;

    static std::shared_ptr<ObjectDetector> MakeInstance();
};