#include "atom.hpp"
#include <iostream>
#include <algorithm>

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

const std::string &JAtom::getElementName() const {
    return ElementsData[static_cast<size_t>(elementType)];
}

void JAtom::setElementType(const ElementType &_elementType) {
    elementType = _elementType;
}

void JAtom::setElementType(const std::string &_elementName) {
    auto it = std::find(ElementsData.begin(), ElementsData.end(), _elementName);
    if (ElementsData.end() == it) {
        std::cerr << _elementName << "not found: In JAtom::setElementType" << std::endl;
        exit(-1);
    }
    elementType = static_cast<ElementType>(std::distance(ElementsData.begin(), it));
}

void JAtom::setElementType(const size_t &_atomicNumber) {
    elementType = static_cast<ElementType>(_atomicNumber);
}

const ElementType &JAtom::getElementType() const {
    return elementType;
}

size_t JAtom::getAtomicNumber() const {
    return static_cast<size_t>(elementType);
}

const size_t &JAtom::getId() const {
    return id;
}

void JAtom::setId(const size_t &_id) {
    id = _id;
}

JAtom::JAtom() {

}

JAtom::JAtom(const size_t &_id, const size_t &_atomicNumber) : id(_id) {
    setElementType(_atomicNumber);
}

const AtomStereo &JAtom::getAtomStereo() const {
    return atomStereo;
}

void JAtom::setAtomStereo(const AtomStereo &_atomStereo) {
    atomStereo = _atomStereo;
}
