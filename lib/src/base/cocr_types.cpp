#include "base/cocr_types.hpp"
#include <iostream>
#include <vector>

std::vector<std::string> ElementsData = {
        "None", "H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne",
        "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",
        "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
        "Ga", "Ge", "As", "Se", "Br", "Kr",
        "Rb", "Sr", "Y", "Zr", "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd",
        "In", "Sn", "Sb", "Te", "I", "Xe",
        "Cs", "Ba", "La", "Ce", "Pr", "Nd", "Pm", "Sm", "Eu", "Gd", "Tb", "Dy",
        "Ho", "Er", "Tm", "Yb", "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt",
        "Au", "Hg", "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
        "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm", "Md", "No",
        "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Ds", "Rg", "Cn"
};
std::string POS_CHARGE_TEXT = "⊕", NEG_CHARGE_TEXT = "㊀";
// FIXME: add all elements you need here
// H B C N O F Si P S Cl Br I
std::unordered_map<ElementType, frac> ElementValenceData = {
        {ElementType::H,  1},
        {ElementType::He, 2},
        {ElementType::B,  3},
        {ElementType::C,  4},
        {ElementType::N,  3},
        {ElementType::O,  2},
        {ElementType::F,  1},
        {ElementType::Si, 4},
        {ElementType::P,  3},
        {ElementType::S,  2},
        {ElementType::Cl, 1},
        {ElementType::Br, 1},
        {ElementType::I,  1}
};

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
            return DetectorClasses::ItemHorizontalStr;
        }
    }
}


const std::string &convertElementTypeToString(const ElementType &_elementType) {
    return ElementsData[static_cast<size_t>(_elementType)];
}

float atomRadius(const ElementType &_element) {
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