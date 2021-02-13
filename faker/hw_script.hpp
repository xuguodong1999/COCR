#ifndef _HW_SCRIPT_HPP_
#define _HW_SCRIPT_HPP_

#include "hw_stroke.hpp"

/**
 * 笔迹：笔画的集合
 */
class HwScript : public HwBase {
    std::vector<HwStroke> mData;
    HwController *hwController;
public:
    float getAvgMaxSize() const;
    size_t size() const;

    void append(const HwScript &_hwScript);


    float getAvgPtsNum() const;

    void clear();

    void setHwController(HwController &_hwController) override;

    void push_back(HwStroke &_stroke);

    void keepIf(const std::function<bool(const cv::Point2f &)> &_cond);

    HwScript(HwController *_hwController = nullptr);

    HwScript(const HwScript &_hwScript) = default;

    HwScript(HwScript &&_hwScript);

    HwScript &operator=(HwScript &&_hwScript) = default;

    void paintTo(cv::Mat &_canvas) const;

    std::optional<cv::Rect2f> getBoundingBox() const;

    void rotateBy(float _angle, const cv::Point2f &_cent);

    void moveBy(const cv::Point2f &_offset);

    void mulK(float _kx, float _ky);

    /**
 * 算一个映射：从【最小矩形】向【等比例的以(p1,p2)为中长轴的矩形】，为所有点做这个映射，指向随机
 * @param p1 起点
 * @param p2 终点
 */
    void castToLine(const cv::Point2f &_p1, const cv::Point2f &_p2, const float _lThresh = 0.05);

    /**
     *
     * @param center 中心
     * @param rx 椭圆水平半径
     * @param ry 椭圆竖直半径
     */
    void castToCircle(const cv::Point2f &_center, float _rx, float _ry);

    /**
     * 构造一个from->to的仿射变换，为所有点执行这个仿射变换
     * @param from 三角形1
     * @param to 三角形2
     */
    void castBy(const std::vector<cv::Point2f> &_from, const std::vector<cv::Point2f> &_to);
};


#endif//_HW_SCRIPT_HPP_
