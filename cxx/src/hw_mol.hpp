#ifndef _HW_MOL_HPP_
#define _HW_MOL_HPP_

#include "hw.hpp"
#include "mol.hpp"
#include <string>
#include <vector>
#include <map>

class MolItem : public ShapeInterface {
    std::vector<std::shared_ptr<ShapeGroup>> symbols;
//    OpenBabel::OBMol obMol;
//    std::map<int, OpenBabel::OBAtom *> aidMap;
    std::string inSmiles;

    void mulK(float kx, float ky) override;

    const JMol &mol;
public:
    /**
     * @param _showCProb 控制碳原子是否显示写出
     * @return 平均图元大小
     */
    float reloadHWData(const float &_showCProb = 0.1);

    void dumpAsDarknet(const char *_topDir, const size_t &_repeatTimes = 1);

    /**
     * 用 JMol 构造一个几何分子类型
     * @param _jmol Construct a MolItem from JMol
     */
    MolItem(const JMol &_jmol);

    void paintTo(cv::Mat &canvas) override;

    /**
     * 获取平行于二维坐标轴的最小边框
     * @return
     */
    const cv::Rect2f getBoundingBox() const override;

    /**
     * 顺时针旋转，绕骨架中心
     * @param angle 角度制
     */
    void rotate(float angle) override;

    void rotateBy(float angle, const cv::Point2f &cent) override;

    /**
     * 将数据点整体移动 offset
     * @param offset
     */
    void move(const cv::Point2f &offset) override;

    /**
     * 将数据点的中心移动到 newCenter
     * @param newCenter
     */
    void moveCenterTo(const cv::Point2f &newCenter) override;

    /**
     * 将数据点的边框左上角移动到 leftTop
     * @param leftTop
     */
    void moveLeftTopTo(const cv::Point2f &leftTop) override;

    /**
     * 将数据点整体缩放到 (w,h)
     * @param w 目标宽度
     * @param h 目标长度
     * @param keepRatio 是否保持比例，为 true 时撑满长边
     */
    void resizeTo(float w, float h, bool keepRatio = true) override;
};

#endif //_HW_MOL_HPP_
