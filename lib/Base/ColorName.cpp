#include "XGD/Base/ColorName.h"
#include "Data/RGBTable.h"

rgb ColorUtil::GetRGB(const ColorName &color) {
    const auto &abc = predefinedColors[(size_t) color];
    return {abc[0], abc[1], abc[2]};
}

rgba ColorUtil::GetRGBA(const ColorName &color) {
    const auto &abc = predefinedColors[(size_t) color];
    return {abc[0], abc[1], abc[2], 255};
}
