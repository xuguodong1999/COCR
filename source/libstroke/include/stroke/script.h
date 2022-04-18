#pragma once

#include "els_stroke_export.h"
#include "stroke/abc.h"
#include "stroke/stroke.h"

/**
 * 笔迹：笔画的集合
 */
class ELS_STROKE_EXPORT HwScript : public HwBase {
    std::vector<HwStroke> mData;
    HwController *hwController;
public:
    std::shared_ptr<HwBase> clone() const override;

    float getAvgMaxSize() const;

    size_t size() const;

    void append(const HwScript &_hwScript);


    float getAvgPtsNum() const;

    void clear();

    void setHwController(HwController &_hwController) override;

    void push_back(HwStroke &_stroke);

    void keepIf(const std::function<bool(const point2f &)> &_cond);

    HwScript(HwController *_hwController = nullptr);

    HwScript(const HwScript &_hwScript) = default;

    HwScript(HwScript &&_hwScript);

    HwScript &operator=(HwScript &&_hwScript) = default;

    HwScript &operator=(const HwScript &_hwScript) = default;

    void paintTo(Mat &_canvas) const;

    std::optional<rectf> getBoundingBox() const;

    void rotateBy(float _angle, const point2f &_cent);

    void moveBy(const point2f &_offset);

    void mulK(float _kx, float _ky);

    /**
    * 算一个映射：从【最小矩形】向【等比例的以(p1,p2)为中长轴的矩形】，为所有点做这个映射，指向随机
    * @param p1 起点
    * @param p2 终点
    */
    void castToLine(const point2f &_p1, const point2f &_p2, const float _lThresh = 0.05);

    /**
     *
     * @param center 中心
     * @param rx 椭圆水平半径
     * @param ry 椭圆竖直半径
     */
    void castToCircle(const point2f &_center, float _rx, float _ry);

    /**
     * 构造一个from->to的仿射变换，为所有点执行这个仿射变换
     * @param from 三角形1
     * @param to 三角形2
     */
    void castBy(const std::vector<point2f> &_from, const std::vector<point2f> &_to);
};