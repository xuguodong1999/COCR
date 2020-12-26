/**
 * This file is a part of COCR Project
 * @author 徐国栋
 */
#include "cocr_types.hpp"
#include <iostream>
#include <vector>

extern std::vector<std::string> ElementsData;

DetectorClasses convertElementTypeToDetectorClasses(const ElementType &_elementType) {
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
            std::cerr << "unsupported convert in convertElementTypeToDetectorClasses: from"
                      << (int) _elementType << std::endl;
            exit(-1);
        }
    }
}


const std::string &convertElementTypeToString(const ElementType &_elementType) {
    return ElementsData[static_cast<size_t>(_elementType)];
}
