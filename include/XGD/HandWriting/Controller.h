#pragma once

#include "xgd_handwriting_export.h"
#include "XGD/Base/ColorName.h"

class XGD_HANDWRITING_EXPORT HwController {
    rgb color;
public:
    explicit HwController(const int &_thickness);

    HwController();

    const rgb &getColor() const;

    int getThickness() const;

    float getRevertColorProb() const;

private:
    int thickness;
    float revertColorProb;
};
