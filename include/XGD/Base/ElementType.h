#pragma once

#include "xgd_base_export.h"
#include "XGD/Base/Fraction.h"
#include "XGD/Base/ColorName.h"
#include "XGD/Base/BondType.h"
#include <unordered_map>
#include <cstddef>
#include <vector>
#include <optional>

// use 0 to represent the so-called super atom in OSRA project
enum class ElementType : size_t {
    SA = 0, H = 1, He, Li, Be, B, C = 6, N = 7, O = 8, F = 9, Ne,
    Na, Mg, Al, Si, P, S = 16, Cl = 17, Ar, K, Ca = 20,
    Sc, Ti, V, Cr, Mn, Fe, Co, Ni, Cu, Zn, Ga, Ge, As, Se, Br, Kr,
    Rb, Sr, Y, Zr, Nb, Mo, Tc, Ru, Rh, Pd, Ag, Cd, In, Sn, Sb, Te, I, Xe,
    Cs, Ba, La, Ce, Pr, Nd, Pm, Sm, Eu, Gd, Tb, Dy, Ho, Er, Tm, Yb, Lu, Hf, Ta, W, Re, Os, Ir, Pt, Au,
    Hg, Tl, Pb, Bi, Po, At, Rn, Fr, Ra, Ac, Th, Pa, U, Np, Pu, Am, Cm, Bk, Cf, Es, Fm, Md, No, Lr, Rf,
    Db, Sg, Bh, Hs, Mt, Ds, Rg, Cn
};

class XGD_BASE_EXPORT ElementUtil {
public:
    static frac GetElementValence(const ElementType &element);

    // radius for UI render, C set to 1
    static float atomRadius(const ElementType &_element);

    static const std::string &GetPosChargeText();

    static const std::string &GetNegChargeText();

    static const std::string &convertElementTypeToString(const ElementType &_elementType);

    static ElementType convertStringToElementType(const std::string &_elementName);

    static float GetElementMass(const ElementType &_elementType);

    static float GetElementElectron(const ElementType &_elementType);

    static ColorName GetElementColor(const ElementType &_elementType);

    static ColorName GetBondColor(const BondType &bondType);

    static std::optional<int> GetElementNebNum(const ElementType &_elementType);

    static std::optional<float> GetElementRadius(const ElementType &_elementType);

    static const std::vector<std::string> &GetElements();
};