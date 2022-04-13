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