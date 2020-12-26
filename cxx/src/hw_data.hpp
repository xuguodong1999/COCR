#ifndef _HW_DATA_HPP_
#define _HW_DATA_HPP_

#include "hw_script.hpp"
#include <map>
#include <string>
#include <vector>

class HwDataSample : public std::vector<std::vector<cv::Point2f>> {
public:
    HwDataSample() = default;

    HwDataSample(std::vector<std::vector<cv::Point2f>> &&_data);

    HwScript toHwScript() const;
};

extern std::string COUCH_SYM_PATH;
enum class ShapeType{
    Line,
    Circle
};
class HwDataLoader {
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

    HwScript GetShape(const ShapeType&_shapeType);

    HwScript GetHwScript(const size_t &_classIndex, const size_t &_sampleIndex);
};

#endif//_HW_DATA_HPP_
