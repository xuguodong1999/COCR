#pragma once

#include "els_stroke_export.h"
#include "base/color_name.h"

class ELS_STROKE_EXPORT HwController {
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
