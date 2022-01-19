#ifndef _COCR_TYPES_HPP_
#define _COCR_TYPES_HPP_

#include "fraction.hpp"
#include "../chem/jatom.hpp"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

using ElementType = xgd::ElementType;
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

enum class AtomStereo {
    None = 0,
    R,
    S
};

extern std::vector<std::string> ElementsData;
extern std::unordered_map<ElementType, frac> ElementValenceData;
extern std::string POS_CHARGE_TEXT, NEG_CHARGE_TEXT;

DetectorClasses convertElementTypeToDetectorClasses(const ElementType &_elementType);

const std::string &convertElementTypeToString(const ElementType &_elementType);

// 返回建议比例，以碳为基准
float atomRadius(const ElementType &_element);

#endif//_COCR_TYPES_HPP_
