#ifndef _ATOM_HPP_
#define _ATOM_HPP_

#include "idmanager.hpp"
#include <string>
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
enum class AtomStereo {
    R,
    S,
    None
};

class JAtom : public IdInterface {
    ElementType elementType;
    inline static std::vector<std::string> ElementsData = {
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
private:
    std::set<size_t> atomIds;// 邻居原子
public:
    ElementType getElementType() const {
        return elementType;
    }

    JAtom(const size_t &_atomicNumber = static_cast<size_t>(ElementType::C)) : elementType(
            static_cast<ElementType>(_atomicNumber)) {
    }

    JAtom(const ElementType &_elementType = ElementType::C)
            : elementType(_elementType) {

    }

    ~JAtom() {

    }

    const std::string &getElementName() {
        return ElementsData[static_cast<size_t>(elementType)];
    }

};

#endif//_ATOM_HPP_