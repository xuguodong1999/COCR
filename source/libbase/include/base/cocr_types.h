#pragma once

#include "els_base_export.h"
#include "base/element_type.h"
#include "base/rect.h"
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>


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

enum class DetectorObjectType {
    SingleLine = 0,
    DoubleLine = 1,
    TripleLine = 2,
    SolidWedge = 3,
    DashWedge = 4,
    WaveLine = 5,
    Circle = 6,
    Text = 7
};

enum class OCRItemType {
    Element,
    Group,
    Line,
    Circle
};

struct ELS_BASE_EXPORT DetectorObject {
    const rectf mRect;
    float prob;
    DetectorObjectType label;
    inline static int maxLabel = 7;
    inline static int minLabel = 0;
    static bool isValidLabel(const int &_label);

    template<class RectFloatType, class ProbFloatType>
    DetectorObject(
            const RectFloatType &_x, const RectFloatType &_y,
            const RectFloatType &_w, const RectFloatType &_h,
            const int &_label, const ProbFloatType &_prob = 1)
            :mRect(point2f{_x, _y}, point2f{_x + _w, _y + _h}),
             label(static_cast<DetectorObjectType>(_label)),
             prob(_prob) {}

    const rectf &asRect() const;

    float x() const;

    float y() const;

    float w() const;

    float h() const;
};


class ELS_BASE_EXPORT DetectorUtil {
public:
    static DetectorClasses convertElementTypeToDetectorClasses(const ElementType &_elementType);
    static BondType toBondType(const DetectorObjectType &_objType);
};