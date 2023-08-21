#pragma once

#include "xgd_handwriting_export.h"
#include "XGD/Base/COCRTypes.h"
#include "XGD/Base/Rect.h"
#include "XGD/HandWriting/Controller.h"
#include "XGD/Vision/Mat.h"
#include <memory>
#include <optional>

class XGD_HANDWRITING_EXPORT HwBase {
protected:
    bool keepDirection;
public:
    HwBase();

    virtual std::shared_ptr<HwBase> clone() const = 0;

    /**
     * set label
     */
    virtual DetectorClasses getItemType() const;

    /**
     * shall we keep item directions when the skeleton is rotated
     * @return
     */
    virtual bool isDirectionKept() const;

    void setKeepDirection(bool _keepDirection = true);

    /**
     * style controller
     */
    inline static HwController baseController;

    virtual void setHwController(HwController &_hwController) = 0;


    virtual void paintTo(Mat &_canvas) const = 0;

    /**
     * return bounding box parallel to axis
     */
    virtual std::optional<rectf> getBoundingBox() const = 0;

    /**
     * rotate by center in deg
     */
    virtual void rotate(float _angle);

    /**
     * rotate by point in deg
     */
    virtual void rotateBy(float _angle, const point2f &_cent) = 0;

    virtual std::optional<point2f> getCenter();

    virtual void moveBy(const point2f &_offset) = 0;


    virtual void moveCenterTo(const point2f &_newCenter);


    virtual void moveLeftTopTo(const point2f &_leftTop);

    virtual void resizeTo(float _w, float _h, bool _keepRatio = true);

    virtual void mulK(float _kx, float _ky) = 0;
};