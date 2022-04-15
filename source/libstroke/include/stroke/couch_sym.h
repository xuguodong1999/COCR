#pragma once

#include "els_stroke_export.h"
#include "stroke/script.h"
#include "base/point2.h"
#include <vector>

enum class ShapeType {
    Line,
    Circle
};

class ELS_STROKE_EXPORT HwDataSample {
    std::vector<std::vector<point2f>> mData;
public:
    HwDataSample() = default;

    HwDataSample(std::vector<std::vector<point2f>> &&_data);

    HwScript toHwScript() const;
};

class ELS_STROKE_EXPORT HwDataLoader {
    std::vector<std::vector<HwDataSample>> mData;

    HwDataLoader();

    ~HwDataLoader() = default;

    HwDataLoader(const HwDataLoader &) = delete;

    const HwDataLoader &operator=(const HwDataLoader &) = delete;

    bool isDataLoaded;

    void LoadCouchDataSet();

public:
    static HwDataLoader &getInstance();

    HwScript GetByIntLabel(int label);

    HwScript GetByStrLabel(const std::string &label);

    HwScript GetShape(const ShapeType &_shapeType);

    HwScript GetHwScript(const size_t &_classIndex, const size_t &_sampleIndex);
};