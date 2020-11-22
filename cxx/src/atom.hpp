#ifndef _ATOM_HPP_
#define _ATOM_HPP_

#include "fraction.hpp"
#include <string>
#include <vector>
#include <unordered_map>

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

enum class AtomStereo {
    None = 0,
    R,
    S
};

extern std::vector<std::string> ElementsData;
extern std::unordered_map<ElementType, frac> ElementValenceData;

class JAtom {
    ElementType elementType;
    size_t id;
    AtomStereo atomStereo;
public:
    const AtomStereo &getAtomStereo() const;

    void setAtomStereo(const AtomStereo &_atomStereo);

    const ElementType &getElementType() const;

    const std::string &getElementName() const;

    size_t getAtomicNumber() const;

    void setElementType(const ElementType &_elementType);

    void setElementType(const std::string &_elementName);

    void setElementType(const size_t &_atomicNumber);

    const size_t &getId() const;

    void setId(const size_t &_id);

    JAtom();

    JAtom(const size_t &_id, const size_t &_atomicNumber);


};

#endif//_ATOM_HPP_