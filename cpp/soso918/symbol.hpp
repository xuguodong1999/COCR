//
// Created by xgd on 2020/9/18.
//

#ifndef PLAY_OPENCV_SYMBOL_HPP
#define PLAY_OPENCV_SYMBOL_HPP

#include "interface.hpp"
#include "shape.hpp"

/**
 * 作为独立分类的符号【20分类】：
 * H,Li,B,C,N,O,F,Na,Mg,Al,Si,P,S,Cl,K,Ca,Br,I
 * 带圈正电荷，带圈负电荷
 * 聚集的元素串、符号串【1分类】，需要由文本识别系统二次识别：
 * CH3COOH,COOH,...,etc.
 */
class Symbol : public ShapeInterface {
public:
    std::vector<Shape> shapes;
    std::string name;
public:
    static std::shared_ptr<Symbol> GetSymbol(const std::string &_symbolType = "");

    Symbol() : name("Symbol") { isRotateAllowed = false; }

    Symbol(const NString &name) {
        isRotateAllowed = false;
        shapes.clear();
        this->name.clear();
        for (auto &c:name) {
            append(c);
            this->name.append(c.second);
        }
    }

    void append(const NChar &c);

    void resizeTo(float w, float h, bool keepRatio = true);

    void moveLeftTopTo(const cv::Point2f &leftTop);

    void moveCenterTo(const cv::Point2f &newCenter);

    void move(const cv::Point2f &offset);

    void rotate(float angle);

    void rotateBy(float angle, const cv::Point2f &cent);

    const cv::Rect2f getBoundingBox() const override;

    virtual void paintTo(cv::Mat &canvas);

    static void setSNormSizeK(float sNormSizeK);

    static void setSSubSizeK(float sSubSizeK);

    static void setSNormOffsetKx(float sNormOffsetKx);

    static void setSNormOffsetKy(float sNormOffsetKy);

    static void setSSubOffsetKx(float sSubOffsetKx);

    static void setSSubOffsetKy(float sSubOffsetKy);

    static void setSAngleK(float sAngleK);
};


#endif //PLAY_OPENCV_SYMBOL_HPP
