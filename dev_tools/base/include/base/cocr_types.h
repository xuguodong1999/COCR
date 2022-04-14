#pragma once

#include "els_base_export.h"
#include "element_type.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>


enum class DetectorClasses {
    ItemEmpty = 0,
    ItemSingleBond,
    ItemDoubleBond,
    ItemTripleBond,
    ItemSolidWedgeBond,
    ItemDashWedgeBond,
    ItemWaveBond,
    ItemCircleBond,
    ItemC,
    ItemH,
    ItemO,
    ItemN,
    ItemP,
    ItemS,
    ItemF,
    ItemCl,
    ItemBr,
    ItemI,
    ItemB,
    ItemHorizontalStr
};

class ELS_BASE_EXPORT DetectorUtil {
public:
    static DetectorClasses convertElementTypeToDetectorClasses(const ElementType &_elementType);
};