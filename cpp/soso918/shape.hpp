//
// Created by xgd on 2020/9/18.
//

#ifndef PLAY_OPENCV_SHAPE_HPP
#define PLAY_OPENCV_SHAPE_HPP

#include "interface.hpp"

class Shape : public ShapeInterface {

public:

    Shape();

    /**
     * 将当前数据画入 canvas
     * @param canvas 目标画布
     */
    virtual void paintTo(cv::Mat &canvas);

    /**
     * 获取平行于二维坐标轴的最小边框
     * @return
     */
    virtual cv::Rect2f getBoundingBox() const;

    /**
     * 顺时针旋转
     * @param angle 角度制
     */
    virtual void rotate(float angle);

    virtual void rotateBy(float angle, const cv::Point2f &cent);

    /**
     * 将数据点整体移动 offset
     * @param offset
     */
    virtual void move(const cv::Point2f &offset);

    /**
     * 将数据点的中心移动到 newCenter
     * @param newCenter
     */
    virtual void moveCenterTo(const cv::Point2f &newCenter);

    /**
     * 将数据点的边框左上角移动到 leftTop
     * @param leftTop
     */
    virtual void moveLeftTopTo(const cv::Point2f &leftTop);

    /**
     * 将数据点整体缩放到 (w,h)
     * @param w 目标宽度
     * @param h 目标长度
     * @param keepRatio 是否保持比例，为 true 时撑满长边
     */
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
    void castBy(const PointArr &from, const PointArr &to);

    void mulK(float kx, float ky) {
        for (auto &stroke:mData) {
            for (auto &p:stroke) {
                p.x *= kx;
                p.y *= ky;
            }
        }
    }

    void rotateTheta(float theta) {
        float x, y;
        for (auto &stroke:mData) {
            for (auto &p:stroke) {
                x = p.x * std::cos(theta) + p.y * std::sin(theta);
                y = p.y * std::cos(theta) - p.x * std::sin(theta);
                p.x = x;
                p.y = y;
            }
        }
    }

    void rotateThetaBy(float theta, const cv::Point2f &cent) {
        float x, y;
        for (auto &stroke:mData) {
            for (auto &p:stroke) {
                x = (p.x - cent.x) * std::cos(theta) + (p.y - cent.y) * std::sin(theta) + cent.x;
                y = (p.y - cent.y) * std::cos(theta) - (p.x - cent.x) * std::sin(theta) + cent.y;
                p.x = x;
                p.y = y;
            }
        }
    }

protected:
    Script mData;
public:
    Script &getData();


};


#endif //PLAY_OPENCV_SHAPE_HPP
