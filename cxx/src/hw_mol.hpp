#ifndef _MOL_HW_HPP_
#define _MOL_HW_HPP_

//#include "hw.hpp"
#include "hw_bond.hpp"
#include "hw_str.hpp"
#include "mol.hpp"
#include <string>
#include <vector>
#include <map>

class HwMol : public HwBase {
    std::vector<std::shared_ptr<HwBase>> mData;
    HwController* hwController;

    const JMol &mol;

    /**
     * @param _pt 控制碳原子是否显示写出
     * @return 平均图元大小
     */
    float reloadHWData(const float &_pt = 0.1);

public:

    void setHwController(HwController &_hwController)override;
    void showOnScreen(const size_t &_repeatTimes = 1);

    /**
     *
     * @param _imgPath C:/soso/JPEGImages/1a43, e.g. index will append as _i.jpg
     * @param _labelPath C:/soso/labels/1a43, e.g. index will append as _i.txt
     * @param _repeatTimes
     */
    void dumpAsDarknet(const std::string &_imgPath, const std::string &_labelPath,
                       const size_t &_repeatTimes = 1);

    /**
     * 用 JMol 构造一个几何分子类型
     * @param _jmol Construct a MolItem from JMol
     */
    HwMol(const JMol &_jmol);

    void paintTo(cv::Mat &_canvas) const override;

    std::optional<cv::Rect2f> getBoundingBox() const override;

    void rotate(float _angle) override;

    void rotateBy(float _angle, const cv::Point2f &_cent)override;

    void moveBy(const cv::Point2f &_offset)override;

    void mulK(float _kx, float _ky)override;

};

#endif //_MOL_HW_HPP_
