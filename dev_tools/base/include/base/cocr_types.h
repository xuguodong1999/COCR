#pragma once
#include "els_base_export.h"
#include "element_type.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

using hash_type = uint64_t;
using node_type = unsigned char;

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
    static DetectorClasses convertElementTypeToDetectorClasses(const ElementType &_elementType);
};