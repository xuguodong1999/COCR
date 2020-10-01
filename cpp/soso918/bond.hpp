//
// Created by xgd on 2020/9/18.
//

#ifndef PLAY_OPENCV_BOND_HPP
#define PLAY_OPENCV_BOND_HPP

#include "utils.hpp"
#include "symbol.hpp"
#include <memory>
#include <iostream>

/**
 * 单个化学键【7分类】：
 * [-1,-7]
 * 圆圈、单键、双键、三键、实楔形键、虚楔形键、波浪线
 */
class Bond : public Symbol {
protected:
    Bond(const std::string &_bondType)
            : bondType(_bondType), mUseHWChar(false), mIsDataGenerated(false) {
        isRotateAllowed=true;
    }

    bool mUseHWChar, mIsDataGenerated;

public:
    void setUseHandWrittenWChar(bool useHandWrittenChar) {
        mUseHWChar = useHandWrittenChar;
    }

    std::string bondType;

    static std::shared_ptr<Bond> GetBond(const std::string &_bondType = "Single");

    virtual void setVertices(const std::vector<cv::Point2f> &pts) = 0;
};

class CircleBond : public Bond {
    cv::Point2f center;
    float r;

public:
    CircleBond(const std::string &_bondType) : Bond(_bondType) {

    }

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
    }
};

class SingleBond : public Bond {
protected:
    cv::Point2f from, to;
public:
    SingleBond(const std::string &_bondType) : Bond(_bondType) {

    }

    virtual void paintTo(cv::Mat &canvas) override;

    void setVertices(const std::vector<cv::Point2f> &pts) override {
        if (pts.size() != 2) {
            std::cerr << "单双三楔波浪线的pts约定为起点和终点,but "
                      << pts.size() << " pts got." << std::endl;
            exit(-1);
        }
        from = pts[0];
        to = pts[1];
    }
};

class DoubleBond : public SingleBond {
public:
    DoubleBond(const std::string &_bondType) : SingleBond(_bondType) {

    }

    void paintTo(cv::Mat &canvas) override;
};

class TripleBond : public SingleBond {
public:
    TripleBond(const std::string &_bondType) : SingleBond(_bondType) {

    }

    void paintTo(cv::Mat &canvas) override;
};

class SolidWedgeBond : public SingleBond {
public:
    SolidWedgeBond(const std::string &_bondType) : SingleBond(_bondType) {

    }

    void paintTo(cv::Mat &canvas) override;
};

class DashWedgeBond : public SingleBond {
public:
    DashWedgeBond(const std::string &_bondType) : SingleBond(_bondType) {

    }

    void paintTo(cv::Mat &canvas) override;
};

class WaveBond : public SingleBond {
public:
    WaveBond(const std::string &_bondType) : SingleBond(_bondType) {

    }

    void paintTo(cv::Mat &canvas) override;
};

#endif //PLAY_OPENCV_BOND_HPP
