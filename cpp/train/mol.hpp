//
// Created by xgd on 2020/9/18.
//

#ifndef PLAY_OPENCV_MOL_HPP
#define PLAY_OPENCV_MOL_HPP

#include "shape.hpp"
#include "shapegroup.hpp"
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/mol.h>
#include <string>
#include <vector>
#include <map>

class Mol : public ShapeInterface {
    std::vector<std::shared_ptr<ShapeGroup>> symbols;
    OpenBabel::OBMol obMol;
    std::map<int, OpenBabel::OBAtom *> aidMap;
    std::string inSmiles;
    void testRing();
    void testDraw();
public:
    Mol();

    ~Mol();

    void paintTo(cv::Mat &canvas) override;

    void addAtom(int id, float x, float y,
                 const std::string &element, int charge);

    void addBond(int from, int to,
                 const std::string &type = "Single",
                 const std::string &stereo = "None");

    void run(const std::string &taskName = "log");

    std::string getStandardSMILES();

    static void LoopOn(const char *filename, const std::string &taskName);

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


#endif //PLAY_OPENCV_MOL_HPP
