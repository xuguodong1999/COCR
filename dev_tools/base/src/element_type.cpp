#include "base/element_type.h"
#include "base/fraction.h"
#include "d/d_element_name.h"
#include "d/d_element_valence.h"
#include <string>

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