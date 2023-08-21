#include "Data/ElementName.h"
#include "Data/ElementValence.h"
#include "Data/ElementMass.h"
#include "Data/ElementElectron.h"
#include "Data/RGBTable.h"
#include "Data/ElementNeb.h"
#include "Data/AtomRadius.h"

#include "XGD/Base/ElementType.h"
#include "XGD/Base/Fraction.h"
#include "XGD/SpdlogStream/Stream.h"

#include <string>
#include <algorithm>

frac ElementUtil::GetElementValence(const ElementType &element) {
    auto it = ElementValenceData.find(element);
    if (it == ElementValenceData.end()) {
        return 0;
    }
    return it->second;
}

float ElementUtil::atomRadius(const ElementType &_element) {
    switch (_element) {
        case ElementType::B:
            return 82. / 77.;
        case ElementType::C:
            return 77. / 77.;
        case ElementType::H:
            return 32. / 77.;
        case ElementType::O:
            return 73. / 77.;
        case ElementType::N:
            return 75. / 77.;
        case ElementType::P:
            return 106. / 77.;
        case ElementType::S:
            return 102. / 77.;
        case ElementType::F:
            return 72. / 77.;
        case ElementType::Cl:
            return 99. / 77.;
        case ElementType::Br:
            return 114. / 77.;
        case ElementType::I:
            return 133. / 77.;
        default:
            return 1.;
    }
}

const std::string &ElementUtil::GetPosChargeText() {
    static std::string s{"⊕"};
    return s;
}

const std::string &ElementUtil::GetNegChargeText() {
    static std::string s{"㊀"};
    return s;
}

const std::string &ElementUtil::convertElementTypeToString(const ElementType &_elementType) {
    return ElementsData[static_cast<size_t>(_elementType)];
}

ElementType ElementUtil::convertStringToElementType(const std::string &_elementName) {
    auto it = std::find(ElementsData.begin(), ElementsData.end(), _elementName);
    if (ElementsData.end() == it) {
        xgd::serr << "convertStringToElementType: miss " << _elementName << std::endl;
        return ElementType::SA;
    }
    return static_cast<ElementType>(std::distance(ElementsData.begin(), it));
}

float ElementUtil::GetElementMass(const ElementType &_elementType) {
    if ((size_t) _elementType >= ELEMENT_MASS_LIST.size()) {
        xgd::serr << "GetElementMass: miss " << (size_t) _elementType << std::endl;
        return 0;
    }
    return ELEMENT_MASS_LIST[(size_t) _elementType];
}

float ElementUtil::GetElementElectron(const ElementType &_elementType) {
    if ((size_t) _elementType >= ELEMENT_ELECTRON_NEG_LIST.size()) {
        xgd::serr << "GetElementMass: miss " << (size_t) _elementType << std::endl;
        return 0;
    }
    return ELEMENT_ELECTRON_NEG_LIST[(size_t) _elementType];
}

ColorName ElementUtil::GetElementColor(const ElementType &_elementType) {
    auto it = atomColorMap.find(_elementType);
    if (it == atomColorMap.end()) {
        return ColorName::rgbAliceBlue;
    }
    return it->second;
}

std::optional<int> ElementUtil::GetElementNebNum(const ElementType &_elementType) {
    auto it = ELEMENT_COMMON_NEB_NUM_MAP.find(_elementType);
    if (ELEMENT_COMMON_NEB_NUM_MAP.end() == it) {
        return std::nullopt;
    }
    return it->second;
}

std::optional<float> ElementUtil::GetElementRadius(const ElementType &_elementType) {
    if ((size_t) _elementType >= ELEMENT_RADIUS_LIST.size() || _elementType == ElementType::SA) {
//        xgd::serr << "GetElementRadius: miss " << (size_t) _elementType;
        return std::nullopt;
    }
    return ELEMENT_RADIUS_LIST[(size_t) _elementType];
}

ColorName ElementUtil::GetBondColor(const BondType &bondType) {
    auto it = bondColorMap.find(bondType);
    if (it == bondColorMap.end()) {
        return ColorName::rgbAliceBlue;
    }
    return it->second;
}

const std::vector<std::string> &ElementUtil::GetElements() {
    return ElementsData;
}

