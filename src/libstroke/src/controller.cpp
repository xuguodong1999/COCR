#include "stroke/controller.h"

const rgb &HwController::getColor() const {
    return color;
}

int HwController::getThickness() const {
    return thickness;
}

HwController::HwController() : HwController(1) {
}

HwController::HwController(const int &_thickness)
        : color(0, 0, 0), thickness(_thickness), revertColorProb(0.5) {
}

float HwController::getRevertColorProb() const {
    return revertColorProb;
}

