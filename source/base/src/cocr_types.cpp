#include "base/cocr_types.h"
#include <iostream>
#include <vector>

DetectorClasses DetectorUtil::convertElementTypeToDetectorClasses(const ElementType &_elementType) {
    switch (_elementType) {
        case ElementType::H:
            return DetectorClasses::ItemH;
        case ElementType::B:
            return DetectorClasses::ItemB;
        case ElementType::C:
            return DetectorClasses::ItemC;
        case ElementType::N:
            return DetectorClasses::ItemN;
        case ElementType::O:
            return DetectorClasses::ItemO;
        case ElementType::F:
            return DetectorClasses::ItemF;
        case ElementType::P:
            return DetectorClasses::ItemP;
        case ElementType::S:
            return DetectorClasses::ItemS;
        case ElementType::Cl:
            return DetectorClasses::ItemCl;
        case ElementType::Br:
            return DetectorClasses::ItemBr;
        case ElementType::I:
            return DetectorClasses::ItemI;
        default: {
            return DetectorClasses::ItemHorizontalStr;
        }
    }
}

bool DetectorObject::isValidLabel(const int &_label) {
    return minLabel <= _label && _label <= maxLabel;
}

const rectf &DetectorObject::asRect() const {
    return mRect;
}

float DetectorObject::x() const {
    return mRect.first.first;
}

float DetectorObject::y() const {
    return mRect.first.second;
}

float DetectorObject::w() const {
    return mRect.second.first - mRect.first.first;
}

float DetectorObject::h() const {
    return mRect.second.second - mRect.first.second;
}
