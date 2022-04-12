#pragma once

#include "chem/bond_type.hpp"
#include "stroke/hw_item.hpp"

class COCR_STROKE_EXPORT HwBond : public HwItem {
protected:
    virtual void loadHwData() = 0;

    inline static float useHwCharProb = 0.8;
public:
    static void setUseHwCharProb(float useHwCharProb);

    static std::shared_ptr<HwBond> GetHwBond(
            const JBondType &_bondType = JBondType::SingleBond);

    virtual void setVertices(const std::vector<cv::Point2f> &_pts) = 0;
};

class COCR_STROKE_EXPORT HwCircleBond : public HwBond {
    cv::Point2f center;
    float rx, ry;

    void loadHwData() override;

public:

    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwCircleBond() = default;

    void setVertices(const std::vector<cv::Point2f> &_pts) override;

    void rotateBy(float _angle, const cv::Point2f &_cent);

    void moveBy(const cv::Point2f &_offset);

    void mulK(float _kx, float _ky);

    void paintTo(cv::Mat &_canvas) const override;
};

class COCR_STROKE_EXPORT HwSingleBond : public HwBond {
protected:
    cv::Point2f from, to;

    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwSingleBond() = default;

    void setVertices(const std::vector<cv::Point2f> &_pts) override;

    void rotateBy(float _angle, const cv::Point2f &_cent);

    void moveBy(const cv::Point2f &_offset);

    void mulK(float _kx, float _ky);
};

class COCR_STROKE_EXPORT HwDoubleBond : public HwSingleBond {

    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwDoubleBond() = default;
};

class COCR_STROKE_EXPORT HwTripleBond : public HwSingleBond {
    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwTripleBond() = default;
};

class COCR_STROKE_EXPORT HwSolidWedgeBond : public HwSingleBond {
    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwSolidWedgeBond() = default;

    void paintTo(cv::Mat &_canvas) const override;
};

class COCR_STROKE_EXPORT HwDashWedgeBond : public HwSingleBond {
    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwDashWedgeBond() = default;
};

class COCR_STROKE_EXPORT HwWaveBond : public HwSingleBond {
    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwWaveBond() = default;
};