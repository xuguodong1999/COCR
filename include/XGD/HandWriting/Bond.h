#pragma once

#include "xgd_handwriting_export.h"
#include "XGD/HandWriting/Item.h"
#include "XGD/Base/BondType.h"

class XGD_HANDWRITING_EXPORT HwBond : public HwItem {
protected:
    virtual void loadHwData() = 0;

    inline static float useHwCharProb = 0.8;
public:
    static void setUseHwCharProb(float useHwCharProb);

    static std::shared_ptr<HwBond> GetHwBond(
            const BondType &_bondType = BondType::SingleBond);

    virtual void setVertices(const std::vector<point2f> &_pts) = 0;
};

class XGD_HANDWRITING_EXPORT HwCircleBond : public HwBond {
    point2f center;
    float rx, ry;

    void loadHwData() override;

public:

    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwCircleBond() = default;

    void setVertices(const std::vector<point2f> &_pts) override;

    void rotateBy(float _angle, const point2f &_cent) override;

    void moveBy(const point2f &_offset) override;

    void mulK(float _kx, float _ky) override;

    void paintTo(Mat &_canvas) const override;
};

class XGD_HANDWRITING_EXPORT HwSingleBond : public HwBond {
protected:
    point2f from, to;

    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwSingleBond() = default;

    void setVertices(const std::vector<point2f> &_pts) override;

    void rotateBy(float _angle, const point2f &_cent) override;

    void moveBy(const point2f &_offset) override;

    void mulK(float _kx, float _ky) override;
};

class XGD_HANDWRITING_EXPORT HwDoubleBond : public HwSingleBond {

    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwDoubleBond() = default;
};

class XGD_HANDWRITING_EXPORT HwTripleBond : public HwSingleBond {
    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwTripleBond() = default;
};

class XGD_HANDWRITING_EXPORT HwSolidWedgeBond : public HwSingleBond {
    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwSolidWedgeBond() = default;

    void paintTo(Mat &_canvas) const override;
};

class XGD_HANDWRITING_EXPORT HwDashWedgeBond : public HwSingleBond {
    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwDashWedgeBond() = default;
};

class XGD_HANDWRITING_EXPORT HwWaveBond : public HwSingleBond {
    void loadHwData() override;

public:
    std::shared_ptr<HwBase> clone() const override;

    DetectorClasses getItemType() const override;

    HwWaveBond() = default;
};