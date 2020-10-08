#ifndef _BOND_HPP_
#define _BOND_HPP_

#include "config.hpp"
#include "shapegroup.hpp"
#include <memory>
#include <iostream>

/**
 * 单个化学键【7分类】：
 * [-1,-7]
 * 圆圈、单键、双键、三键、实楔形键、虚楔形键、波浪线
 */
class Bond : public ShapeGroup {
protected:
    Bond(const std::string &_bondType)
            : mUseHWChar(true) {
        isRotateAllowed = true;
        name = _bondType;
    }

    bool mUseHWChar;

    virtual void updateShapes() = 0;

public:
    void setUseHandWrittenWChar(bool useHandWrittenChar) {
        mUseHWChar = useHandWrittenChar;
    }

    static std::shared_ptr<Bond> GetBond(const std::string &_bondType = "Single");

    virtual void setVertices(const std::vector<cv::Point2f> &pts) = 0;
};

class CircleBond : public Bond {
    cv::Point2f center;
    float r;

    void updateShapes() override;

public:
    CircleBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;

    void setVertices(const std::vector<cv::Point2f> &pts) override {
        if (pts.size() < 2) {
            std::cerr << "圆键的pts约定为多边形顶点,but "
                      << pts.size() << " pts got." << std::endl;
            exit(-1);
        }
        center.x = center.y = 0;
        for (auto &pt:pts)
            center += pt;
        center.x /= pts.size();
        center.y /= pts.size();
        r = 0;
        for (auto &pt:pts)
            r += distance(center, pt);
        r /= pts.size();
        updateShapes();
    }
};

class SingleBond : public Bond {
    virtual void updateShapes() override;

protected:
    cv::Point2f from, to;
public:
    SingleBond(const std::string &_bondType);

    virtual void paintTo(cv::Mat &canvas) override;

    void setVertices(const std::vector<cv::Point2f> &pts) override {
        if (pts.size() != 2) {
            std::cerr << "单双三楔波浪线的pts约定为起点和终点,but "
                      << pts.size() << " pts got." << std::endl;
            exit(-1);
        }
        from = pts[0];
        to = pts[1];
        updateShapes();
    }
};

class DoubleBond : public SingleBond {
    void updateShapes() override;

public:
    DoubleBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class TripleBond : public SingleBond {
    void updateShapes() override;

public:
    TripleBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class SolidWedgeBond : public SingleBond {
    void updateShapes() override;

public:
    SolidWedgeBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class DashWedgeBond : public SingleBond {
    void updateShapes() override;

public:
    DashWedgeBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

class WaveBond : public SingleBond {
    void updateShapes() override;

public:
    WaveBond(const std::string &_bondType);

    void paintTo(cv::Mat &canvas) override;
};

#endif //_BOND_HPP_
