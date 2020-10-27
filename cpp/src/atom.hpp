#ifndef _ATOM_HPP_
#define _ATOM_HPP_

#include "config.hpp"
#include <set>
enum class ElementType {
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

class JAtom {
    size_t id;
    ElementType elementType;
    std::set<size_t> atomIds;
public:
    JAtom(const size_t &_atomicNumber = static_cast<size_t>(ElementType::C))
            : elementType(static_cast<ElementType>(_atomicNumber)) {
    }

    JAtom(const ElementType &_elementType = ElementType::C)
            : elementType(_elementType) {

    }

    const std::string&getElementName(){
        return ElementsData[static_cast<size_t>(elementType)];
    }

    const size_t &getId();
};

#endif//_ATOM_HPP_