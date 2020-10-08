#ifndef _SHAPE_HPP_
#define _SHAPE_HPP_

#include "config.hpp"
#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>

const cv::Scalar cvBlack = cv::Scalar(0, 0, 0);
const cv::Scalar cvWhite = cv::Scalar(255, 255, 255);
const cv::Scalar cvBlue = cv::Scalar(255, 0, 0);
const cv::Scalar cvRed = cv::Scalar(0, 0, 255);

using Point = cv::Point2f;
using Stroke = v<Point>;
using Script = v<Stroke>;

class ShapeInterface {
public:
    /**
     * 将当前数据画入 canvas
     * @param canvas 目标画布
     */
    virtual void paintTo(cv::Mat &canvas) = 0;

    /**
     * 获取平行于二维坐标轴的最小边框
     * @return
     */
    virtual const cv::Rect2f getBoundingBox() const = 0;

    /**
     * 顺时针旋转，绕图元中心
     * @param angle 角度制
     */
    virtual void rotate(float angle) = 0;

    /**
     * 顺时针旋转，绕点 cent
     * @param angle 角度制
     */
    virtual void rotateBy(float angle, const Point &cent) = 0;

    virtual const Point getCenter();

    /**
     * 将数据点整体移动 offset
     * @param offset
     */
    virtual void move(const Point &offset) = 0;

    /**
     * 将数据点的中心移动到 newCenter
     * @param newCenter
     */
    virtual void moveCenterTo(const Point &newCenter) = 0;

    /**
     * 将数据点的边框左上角移动到 leftTop
     * @param leftTop
     */
    virtual void moveLeftTopTo(const Point &leftTop) = 0;

    /**
     * 将数据点整体缩放到 (w,h)
     * @param w 目标宽度
     * @param h 目标长度
     * @param keepRatio 是否保持比例，为 true 时撑满长边
     */
    virtual void resizeTo(float w, float h, bool keepRatio = true) = 0;

    /**
     * 为所有点数乘 (kx,ky)
     * @param kx
     * @param ky
     */
    virtual void mulK(float kx, float ky) = 0;

    bool IsRotateAllowed() const { return isRotateAllowed; }

protected:
    bool isRotateAllowed;

    ShapeInterface();
};

class Shape : public ShapeInterface {
public:
    Shape();

    virtual void paintTo(cv::Mat &canvas);

    virtual const cv::Rect2f getBoundingBox() const override;

    virtual void rotate(float angle);

    virtual void rotateBy(float angle, const cv::Point2f &cent);

    virtual void move(const cv::Point2f &offset);

    virtual void moveCenterTo(const cv::Point2f &newCenter);

    virtual void moveLeftTopTo(const cv::Point2f &leftTop);

    virtual void resizeTo(float w, float h, bool keepRatio = true);

    /**
     * 算一个映射：从【最小矩形】向【等比例的以(p1,p2)为中长轴的矩形】，为所有点做这个映射，指向随机
     * @param p1 起点
     * @param p2 终点
     */
    void castToLine(const cv::Point2f &p1, const cv::Point2f &p2, const float lThresh = 0.05);

    /**
     *
     * @param center 中心
     * @param rx 椭圆水平半径
     * @param ry 椭圆竖直半径
     */
    void castToCircle(const cv::Point2f &center, float rx, float ry);

    /**
     * 构造一个from->to的仿射变换，为所有点执行这个仿射变换
     * @param from 三角形1
     * @param to 三角形2
     */
    void castBy(const v<Point> &from, const v<Point> &to);

    void mulK(float kx, float ky);

    void rotateTheta(float theta);

    void rotateThetaBy(float theta, const cv::Point2f &cent);

public:
    Script mData;
    int mLabel;
public:
    Script &getData();
};

cv::Rect2f getRect(const Stroke &stroke);

cv::Rect2f getRect(const Script &script);

template<typename _Tp>
inline _Tp distance(const cv::Point_<_Tp> &p1, const cv::Point_<_Tp> &p2) {
    return std::sqrt(std::pow(p1.x - p2.x, 2) + std::pow(p1.y - p2.y, 2));
}

#endif//_SHAPE_HPP_