#pragma once
#include "els_base_export.h"
#include "base/fraction.h"
#include <unordered_map>
#include <cstddef>

// 约定：元素类型 0 表示 OCRA 所谓的超原子、即未解析的字符串
enum class ElementType : size_t {
    SA = 0, H = 1, He, Li, Be, B, C = 6, N = 7, O = 8, F = 9, Ne,
    Na, Mg, Al, Si, P, S = 16, Cl = 17, Ar, K, Ca = 20,
    Sc, Ti, V, Cr, Mn, Fe, Co, Ni, Cu, Zn, Ga, Ge, As, Se, Br, Kr,
    Rb, Sr, Y, Zr, Nb, Mo, Tc, Ru, Rh, Pd, Ag, Cd, In, Sn, Sb, Te, I, Xe,
    Cs, Ba, La, Ce, Pr, Nd, Pm, Sm, Eu, Gd, Tb, Dy, Ho, Er, Tm, Yb, Lu, Hf, Ta, W, Re, Os, Ir, Pt, Au,
    Hg, Tl, Pb, Bi, Po, At, Rn, Fr, Ra, Ac, Th, Pa, U, Np, Pu, Am, Cm, Bk, Cf, Es, Fm, Md, No, Lr, Rf,
    Db, Sg, Bh, Hs, Mt, Ds, Rg, Cn
};

class ELS_BASE_EXPORT ElementUtil{
public:
    static frac GetElementValence(const ElementType&element);
    // 返回建议比例，以碳为基准
    static float atomRadius(const ElementType &_element);
    static const std::string&GetPosChargeText();
    static const std::string&GetNegChargeText();
    static const std::string &convertElementTypeToString(const ElementType &_elementType);
};