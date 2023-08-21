#pragma once

#include "xgd_handwriting_export.h"
#include "XGD/HandWriting/Script.h"
#include "XGD/Base/Vec2.h"
#include <vector>

enum class ShapeType {
    Line,
    Circle
};

class XGD_HANDWRITING_EXPORT HwDataSample {
    std::vector<std::vector<point2f>> mData;
public:
    HwDataSample() = default;

    HwDataSample(std::vector<std::vector<point2f>> &&_data);

    HwScript toHwScript() const;
};

class XGD_HANDWRITING_EXPORT HwDataLoader {
    std::vector<std::vector<HwDataSample>> mData;

    HwDataLoader();

    ~HwDataLoader() = default;

    HwDataLoader(const HwDataLoader &) = delete;

    const HwDataLoader &operator=(const HwDataLoader &) = delete;

    bool isDataLoaded;

    void LoadCouchDataSet(const std::string &);

public:
    static HwDataLoader &getInstance();

    HwScript GetByIntLabel(int label);

    HwScript GetByStrLabel(const std::string &label);

    HwScript GetShape(const ShapeType &_shapeType);

    HwScript GetHwScript(const size_t &_classIndex, const size_t &_sampleIndex);
};