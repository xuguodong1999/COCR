#ifndef _JATOM_HPP_
#define _JATOM_HPP_

#include <vector>
#include <string>
#include <unordered_map>

namespace xgd {
    enum class ElementType : size_t {
        None = 0, H = 1, He, Li, Be, B, C = 6, N = 7, O = 8, F = 9, Ne,
        Na, Mg, Al, Si, P, S = 16, Cl = 17, Ar, K, Ca = 20,
        Sc, Ti, V, Cr, Mn, Fe, Co, Ni, Cu, Zn, Ga, Ge, As, Se, Br, Kr,
        Rb, Sr, Y, Zr, Nb, Mo, Tc, Ru, Rh, Pd, Ag, Cd, In, Sn, Sb, Te, I, Xe,
        Cs, Ba, La, Ce, Pr, Nd, Pm, Sm, Eu, Gd, Tb, Dy, Ho, Er, Tm, Yb, Lu, Hf, Ta, W, Re, Os, Ir, Pt, Au,
        Hg, Tl, Pb, Bi, Po, At, Rn, Fr, Ra, Ac, Th, Pa, U, Np, Pu, Am, Cm, Bk, Cf, Es, Fm, Md, No, Lr, Rf,
        Db, Sg, Bh, Hs, Mt, Ds, Rg, Cn
    };

    extern std::vector<std::string> ELEMENT_NAME_LIST;
    extern std::unordered_map<std::string, ElementType> STR_ELEMENT_SET;
}
#endif//_JATOM_HPP_
