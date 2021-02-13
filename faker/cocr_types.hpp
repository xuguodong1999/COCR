#ifndef _COCR_TYPES_HPP_
#define _COCR_TYPES_HPP_

#include "fraction.hpp"

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

using hash_type = uint64_t;
using node_type = unsigned char;


enum class DetectorClasses {
    ItemEmpty = 0,
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
    ItemSingleBond,
    ItemDoubleBond,
    ItemTripleBond,
    ItemSolidWedgeBond,
    ItemDashWedgeBond,
    ItemCircleBond,
    ItemWaveBond,
    ItemHorizontalStr
};

enum class ElementType : size_t {
    None = 0, H = 1, He, Li, Be, B, C = 6, N = 7, O = 8, F = 9, Ne,
    Na, Mg, Al, Si, P, S = 16, Cl = 17, Ar, K, Ca = 20,
    Sc, Ti, V, Cr, Mn, Fe, Co, Ni, Cu, Zn,
    Ga, Ge, As, Se, Br, Kr,
    Rb, Sr, Y, Zr, Nb, Mo, Tc, Ru, Rh, Pd, Ag, Cd,
    In, Sn, Sb, Te, I, Xe,
    Cs, Ba, La, Ce, Pr, Nd, Pm, Sm, Eu, Gd, Tb, Dy,
    Ho, Er, Tm, Yb, Lu, Hf, Ta, W, Re, Os, Ir, Pt,
    Au, Hg, Tl, Pb, Bi, Po, At, Rn, Fr, Ra, Ac, Th,
    Pa, U, Np, Pu, Am, Cm, Bk, Cf, Es, Fm, Md, No,
    Lr, Rf, Db, Sg, Bh, Hs, Mt, Ds, Rg, Cn
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
