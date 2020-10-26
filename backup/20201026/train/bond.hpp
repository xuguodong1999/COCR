#ifndef _BOND_HPP_
#define _BOND_HPP_

#include "config.hpp"
#include "shapegroup.hpp"


/**
 * 单个化学键【7分类】：
 * [-1,-7]
 * 圆圈、单键、双键、三键、实楔形键、虚楔形键、波浪线
 */
class Bond : public ShapeGroup {
protected:
    Bond(const string &_bondType);

    bool mUseHWChar;
    bool isLatest;

    virtual void updateShapes();

public:

    void setUseHandWrittenWChar(bool useHandWrittenChar);

    static shared_ptr<Bond> GetBond(const string &_bondType = "Single");

    virtual void setVertices(const vector<Point> &pts) = 0;
};

class CircleBond : public Bond {
    Point center;
    float r;

    void updateShapes() override;

public:
    CircleBond(const string &_bondType);

    const cv::Rect2f getBoundingBox() const override;

    void mulK(float kx, float ky);

    void resizeTo(float w, float h, bool keepRatio = true) override;

    void moveLeftTopTo(const Point &leftTop) override;

    void moveCenterTo(const Point &newCenter) override;

    void move(const Point &offset) override;

    void rotate(float angle) override;

    void rotateBy(float angle, const Point &cent) override;

    void paintTo(cv::Mat &canvas) override;

    void setVertices(const vector<Point> &pts) override;
};

class SingleBond : public Bond {
    virtual void updateShapes() override;

protected:
    Point from, to;
public:
    const cv::Rect2f getBoundingBox() const override;

    SingleBond(const string &_bondType);

    void mulK(float kx, float ky);

    void resizeTo(float w, float h, bool keepRatio = true) override;

    void moveLeftTopTo(const Point &leftTop) override;

    void moveCenterTo(const Point &newCenter) override;

    void move(const Point &offset) override;

    void rotate(float angle) override;

    void rotateBy(float angle, const Point &cent) override;

    virtual void paintTo(cv::Mat &canvas) override;

    void setVertices(const vector<Point> &pts) override;
};

class DoubleBond : public SingleBond {
    void updateShapes() override;

public:
    DoubleBond(const string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class TripleBond : public SingleBond {
    void updateShapes() override;

public:
    TripleBond(const string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class SolidWedgeBond : public SingleBond {
    void updateShapes() override;

public:
    SolidWedgeBond(const string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class DashWedgeBond : public SingleBond {
    void updateShapes() override;

public:
    DashWedgeBond(const string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class WaveBond : public SingleBond {
    void updateShapes() override;

public:
    WaveBond(const string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

#endif //_BOND_HPP_
