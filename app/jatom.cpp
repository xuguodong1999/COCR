#include "jatom.hpp"
#include "color_types.hpp"
#include <vector>
#include <string>
#include <unordered_map>

#include <QDebug>

using xgd::ElementType;
using xgd::ColorName;
std::vector<float> xgd::ELEMENT_MASS_LIST = {
        0, 1.00795, 4.0026, 6.9412, 9.01218, 10.8117, 12.0108, 14.0067, 15.9994, 18.9984, 20.1798, 22.9898, 24.3051,
        26.9815, 28.0855, 30.9738, 32.0655, 35.4532, 39.9481, 39.0983, 40.0784, 44.9559, 47.8671, 50.9415, 51.9962,
        54.938, 55.8452, 58.9332, 58.6934, 63.5463, 65.394, 69.7231, 72.641, 74.9216, 78.963, 79.9041, 83.7982, 85.4678,
        87.621, 88.9059, 91.2242, 92.9064, 95.942, 97.9072, 101.072, 102.906, 106.421, 107.868, 112.412, 114.818,
        118.711, 121.76, 127.603, 126.904, 131.294, 132.905, 137.328, 137.328, 140.116, 140.908, 144.242, 145, 150.362,
        151.964, 157.253, 158.925, 162.5, 164.93, 167.259, 168.934, 173.043, 174.967, 178.492, 180.948, 183.841,
        186.207, 190.233, 192.217, 195.085, 196.967, 200.592, 204.383, 207.21, 208.98, 208.982, 209.987, 222.018, 223,
        226, 227, 232.038, 231.036, 238.029, 238.849, 242.88, 244.859, 246.911, 248.927, 252.958, 253.966, 259.005,
        260.012, 261.02, 264.044, 269.083, 270.09, 273.114, 274.122, 272.106, 278.153, 283.192, 282.184, 287.223,
};
std::vector<float> xgd::ELEMENT_ELECTRON_NEG_LIST = {
        0, 2.2, 3.89, 0.98, 1.57, 2.04, 2.55, 3.04, 3.44, 3.98, 3.67, 0.93, 1.31, 1.61, 1.9, 2.19, 2.58, 3.16, 3.3,
        0.82, 1, 1.36, 1.54, 1.63, 1.66, 1.55, 1.83, 1.88, 1.91, 1.9, 1.65, 1.81, 2.01, 2.18, 2.55, 2.96, 3, 0.82, 0.95,
        1.22, 1.33, 1.6, 2.16, 1.9, 2.2, 2.28, 2.2, 1.93, 1.69, 1.78, 1.96, 2.05, 2.1, 2.66, 2.67, 0.97, 0.89, 1.1,
        1.12, 1.13, 1.14, 1.13, 1.17, 1.2, 1.2, 1.1, 1.22, 1.23, 1.24, 1.25, 1.1, 1.27, 1.3, 1.5, 2.36, 1.9, 2.2, 2.2,
        2.28, 2.54, 2, 1.62, 2.33, 2.02, 2, 2.2, 2.2, 0.7, 0.9, 1.1, 1.3, 1.5, 1.38, 1.36, 1.28, 1.13, 1.28, 1.3, 1.3,
        1.3, 1.3, 1.3, 1.3, 1.291, 1.3, 1.5, 2.36, 1.9, 2.2, 2.2, 2.28, 2.54, 2,
};
std::vector<float> xgd::ELEMENT_RADIUS_LIST = {
        0, 32, 93, 123, 90, 82, 77, 75, 73, 72, 71, 154, 136, 118, 111, 106, 102, 99, 98, 203, 174, 144, 132, 122, 118,
        117, 117, 116, 115, 117, 125, 126, 122, 120, 116, 114, 112, 216, 191, 162, 145, 134, 130, 127, 125, 125, 128,
        134, 148, 144, 141, 140, 136, 133, 131, 235, 198, 169, 165, 165, 164, 163, 162, 185, 161, 159, 159, 158, 157,
        156, 174, 156, 144, 134, 130, 128, 126, 127, 130, 134, 149, 148, 147, 146, 146, 145, 0, 0, 0, 0, 165, 0, 142,
};
std::vector<std::string> xgd::ELEMENT_NAME_LIST = {
        "None",
        "H",
        "He",
        "Li",
        "Be",
        "B",
        "C",
        "N",
        "O",
        "F",
        "Ne",
        "Na",
        "Mg",
        "Al",
        "Si",
        "P",
        "S",
        "Cl",
        "Ar",
        "K",
        "Ca",
        "Sc",
        "Ti",
        "V",
        "Cr",
        "Mn",
        "Fe",
        "Co",
        "Ni",
        "Cu",
        "Zn",
        "Ga",
        "Ge",
        "As",
        "Se",
        "Br",
        "Kr",
        "Rb",
        "Sr",
        "Y",
        "Zr",
        "Nb",
        "Mo",
        "Tc",
        "Ru",
        "Rh",
        "Pd",
        "Ag",
        "Cd",
        "In",
        "Sn",
        "Sb",
        "Te",
        "I",
        "Xe",
        "Cs",
        "Ba",
        "La",
        "Ce",
        "Pr",
        "Nd",
        "Pm",
        "Sm",
        "Eu",
        "Gd",
        "Tb",
        "Dy",
        "Ho",
        "Er",
        "Tm",
        "Yb",
        "Lu",
        "Hf",
        "Ta",
        "W",
        "Re",
        "Os",
        "Ir",
        "Pt",
        "Au",
        "Hg",
        "Tl",
        "Pb",
        "Bi",
        "Po",
        "At",
        "Rn",
        "Fr",
        "Ra",
        "Ac",
        "Th",
        "Pa",
        "U",
        "Np",
        "Pu",
        "Am",
        "Cm",
        "Bk",
        "Cf",
        "Es",
        "Fm",
        "Md",
        "No",
        "Lr",
        "Rf",
        "Db",
        "Sg",
        "Bh",
        "Hs",
        "Mt",
        "Ds",
        "Rg",
        "Cn"
};

std::unordered_map<std::string, ElementType> xgd::STR_ELEMENT_SET = {
        {"None", ElementType::None},
        {"H",    ElementType::H},
        {"He",   ElementType::He},
        {"Li",   ElementType::Li},
        {"Be",   ElementType::Be},
        {"B",    ElementType::B},
        {"C",    ElementType::C},
        {"N",    ElementType::N},
        {"O",    ElementType::O},
        {"F",    ElementType::F},
        {"Ne",   ElementType::Ne},
        {"Na",   ElementType::Na},
        {"Mg",   ElementType::Mg},
        {"Al",   ElementType::Al},
        {"Si",   ElementType::Si},
        {"P",    ElementType::P},
        {"S",    ElementType::S},
        {"Cl",   ElementType::Cl},
        {"Ar",   ElementType::Ar},
        {"K",    ElementType::K},
        {"Ca",   ElementType::Ca},
        {"Sc",   ElementType::Sc},
        {"Ti",   ElementType::Ti},
        {"V",    ElementType::V},
        {"Cr",   ElementType::Cr},
        {"Mn",   ElementType::Mn},
        {"Fe",   ElementType::Fe},
        {"Co",   ElementType::Co},
        {"Ni",   ElementType::Ni},
        {"Cu",   ElementType::Cu},
        {"Zn",   ElementType::Zn},
        {"Ga",   ElementType::Ga},
        {"Ge",   ElementType::Ge},
        {"As",   ElementType::As},
        {"Se",   ElementType::Se},
        {"Br",   ElementType::Br},
        {"Kr",   ElementType::Kr},
        {"Rb",   ElementType::Rb},
        {"Sr",   ElementType::Sr},
        {"Y",    ElementType::Y},
        {"Zr",   ElementType::Zr},
        {"Nb",   ElementType::Nb},
        {"Mo",   ElementType::Mo},
        {"Tc",   ElementType::Tc},
        {"Ru",   ElementType::Ru},
        {"Rh",   ElementType::Rh},
        {"Pd",   ElementType::Pd},
        {"Ag",   ElementType::Ag},
        {"Cd",   ElementType::Cd},
        {"In",   ElementType::In},
        {"Sn",   ElementType::Sn},
        {"Sb",   ElementType::Sb},
        {"Te",   ElementType::Te},
        {"I",    ElementType::I},
        {"Xe",   ElementType::Xe},
        {"Cs",   ElementType::Cs},
        {"Ba",   ElementType::Ba},
        {"La",   ElementType::La},
        {"Ce",   ElementType::Ce},
        {"Pr",   ElementType::Pr},
        {"Nd",   ElementType::Nd},
        {"Pm",   ElementType::Pm},
        {"Sm",   ElementType::Sm},
        {"Eu",   ElementType::Eu},
        {"Gd",   ElementType::Gd},
        {"Tb",   ElementType::Tb},
        {"Dy",   ElementType::Dy},
        {"Ho",   ElementType::Ho},
        {"Er",   ElementType::Er},
        {"Tm",   ElementType::Tm},
        {"Yb",   ElementType::Yb},
        {"Lu",   ElementType::Lu},
        {"Hf",   ElementType::Hf},
        {"Ta",   ElementType::Ta},
        {"W",    ElementType::W},
        {"Re",   ElementType::Re},
        {"Os",   ElementType::Os},
        {"Ir",   ElementType::Ir},
        {"Pt",   ElementType::Pt},
        {"Au",   ElementType::Au},
        {"Hg",   ElementType::Hg},
        {"Tl",   ElementType::Tl},
        {"Pb",   ElementType::Pb},
        {"Bi",   ElementType::Bi},
        {"Po",   ElementType::Po},
        {"At",   ElementType::At},
        {"Rn",   ElementType::Rn},
        {"Fr",   ElementType::Fr},
        {"Ra",   ElementType::Ra},
        {"Ac",   ElementType::Ac},
        {"Th",   ElementType::Th},
        {"Pa",   ElementType::Pa},
        {"U",    ElementType::U},
        {"Np",   ElementType::Np},
        {"Pu",   ElementType::Pu},
        {"Am",   ElementType::Am},
        {"Cm",   ElementType::Cm},
        {"Bk",   ElementType::Bk},
        {"Cf",   ElementType::Cf},
        {"Es",   ElementType::Es},
        {"Fm",   ElementType::Fm},
        {"Md",   ElementType::Md},
        {"No",   ElementType::No},
        {"Lr",   ElementType::Lr},
        {"Rf",   ElementType::Rf},
        {"Db",   ElementType::Db},
        {"Sg",   ElementType::Sg},
        {"Bh",   ElementType::Bh},
        {"Hs",   ElementType::Hs},
        {"Mt",   ElementType::Mt},
        {"Ds",   ElementType::Ds},
        {"Rg",   ElementType::Rg},
        {"Cn",   ElementType::Cn}
};

const std::string &xgd::convertElementTypeToName(const ElementType &_type) {
    return ELEMENT_NAME_LIST[static_cast<size_t>(_type)];
}

ElementType xgd::convertNameToElementType(const std::string &_name) {
    auto it = STR_ELEMENT_SET.find(_name);
    if (it != STR_ELEMENT_SET.end())return it->second;
    return ElementType::None;
}

const std::string &xgd::JAtom::getName() const {
    return convertElementTypeToName(type);
}

const ElementType &xgd::JAtom::getType() const {
    return type;
}

void xgd::JAtom::set2D(const float &_x, const float &_y) {
    x = _x;
    y = _y;
}

void xgd::JAtom::set3D(const float &_x, const float &_y, const float &_z) {
    xx = _x;
    yy = _y;
    zz = _z;
}

int xgd::JAtom::getCharge() const {
    return charge;
}

size_t xgd::JAtom::getId() const {
    return id;
}

xgd::JAtom::JAtom(const size_t &_id, const ElementType &_element, const float &_x, const float &_y)
        : id(_id), type(_element), x(_x), y(_y), xx(0), yy(0), zz(0), charge(0), mIsImplicit(false) {

}

int xgd::JAtom::getAtomicNumber() const {
    return static_cast<size_t>(getType());
}

float xgd::JAtom::getMass() const {
    size_t an = getAtomicNumber();
    if (an >= ELEMENT_MASS_LIST.size()) { return 0; }
    return ELEMENT_MASS_LIST[an];
}

float xgd::JAtom::getElectronNeg() const {
    size_t an = getAtomicNumber();
    if (an >= ELEMENT_ELECTRON_NEG_LIST.size()) { return 0; }
    return ELEMENT_ELECTRON_NEG_LIST[an];
}

float xgd::JAtom::getRadius() const {
    size_t an = getAtomicNumber();
    if (an >= ELEMENT_RADIUS_LIST.size()) { return 0; }
    float r = ELEMENT_RADIUS_LIST[an];
    // FIXME: 范德华半径数据有缺失
    if (r < ELEMENT_RADIUS_LIST[1]) { return sDefaultRadius; }
    return r;
}

float xgd::JAtom::getDefaultRadius() {
    return sDefaultRadius;
}

bool xgd::JAtom::isImplicit() const {
    return mIsImplicit;
}

void xgd::JAtom::setImplicit(bool _isImplicit) {
    JAtom::mIsImplicit = _isImplicit;
}

static std::unordered_map<ElementType, ColorName> colorMap = {
        {ElementType::H,  ColorName::rgbLightBlue},
        {ElementType::B,  ColorName::rgbLightCyan},
        {ElementType::C,  ColorName::rgbOrange},
        {ElementType::N,  ColorName::rgbYellow},
        {ElementType::O,  ColorName::rgbDarkSlateBlue},
        {ElementType::F,  ColorName::rgbLightSeaGreen},
        {ElementType::P,  ColorName::rgbDarkGrey},
        {ElementType::S,  ColorName::rgbDarkGoldenrod},
        {ElementType::Cl, ColorName::rgbLightGreen},
        {ElementType::Br, ColorName::rgbDarkRed},
        {ElementType::I,  ColorName::rgbPurple}
};

QColor xgd::getColor(const ElementType &_element) {
    auto it = colorMap.find(_element);
    if (colorMap.end() == it) {
        return qColor(ColorName::rgbPink);
    }
    return qColor(it->second);
}