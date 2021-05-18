#ifndef _MOL_HW_HPP_
#define _MOL_HW_HPP_

#include "hw_bond.hpp"
#include "hw_str.hpp"
#include "../chem/jmol.hpp"
#include <string>
#include <vector>
#include <map>

/**
 * 向JMol填充图元
 */
class HwMol : public HwBase {
    std::vector<std::shared_ptr<HwBase>> mData;
    HwController *hwController;
    std::shared_ptr<xgd::JMol> mol;
    std::unordered_map<size_t, size_t> hwToAtomMap;
    float avgSize;

    /**
     * @param _explicitCarbonProb 控制碳原子是否显示写出
     * @return 平均图元大小
     */
    float reloadHWData(const float &_explicitCarbonProb = 0.1);

public:

    std::shared_ptr<HwBase> clone() const;

    void setHwController(HwController &_hwController) override;

    std::vector<cv::Mat> showOnScreen(const size_t &_repeatTimes = 1, bool _showBox = false);

    /**
     * 用 JMol 构造一个几何分子类型
     * @param _jmol Construct a MolItem from JMol
     */
    HwMol(std::shared_ptr<xgd::JMol> mol,
          HwController *_hwController = nullptr);

    void paintTo(cv::Mat &_canvas) const override;

    std::optional<cv::Rect2f> getBoundingBox() const override;

    void rotate(float _angle) override;

    void rotateBy(float _angle, const cv::Point2f &_cent) override;

    void moveBy(const cv::Point2f &_offset) override;

    void mulK(float _kx, float _ky) override;

};

#endif //_MOL_HW_HPP_
