#ifndef _XGD_JATOM_HPP_
#define _XGD_JATOM_HPP_

//#include "jnode.hpp"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <QColor>
#include <QString>

using id_type = size_t;

namespace xgd {
    class JMol;

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

    extern std::vector<std::string> ELEMENT_NAME_LIST;
    extern std::vector<float> ELEMENT_ELECTRON_NEG_LIST;
    extern std::vector<float> ELEMENT_MASS_LIST;
    extern std::vector<float> ELEMENT_RADIUS_LIST;
    extern std::unordered_map<std::string, ElementType> STR_ELEMENT_MAP;
    extern std::unordered_map<ElementType, int> ELEMENT_COMMON_NEB_NUM_MAP;

    class JAtom {
        ElementType type;
        int charge;

    private:
        inline static const float sDefaultRadius = 100;
        id_type id;
        bool mIsImplicit;
        std::string name;
    public:
        void setCharge(int charge);

        bool isSuperAtom() const;

        float x, y, xx, yy, zz;
        float x0, y0, x1, y1;

        static float getDefaultRadius();

        JAtom(const size_t &_id, const ElementType &_element, const float &_x = 0, const float &_y = 0);

        JAtom(const size_t &_id, const std::string &_name, const float &_x0 = 0, const float &_y0 = 0,
              const float &_x1 = 0, const float &_y1 = 0);

        bool isImplicit() const;

        void setImplicit(bool _isImplicit = true);

        void setId(const id_type &_id);

        id_type getId();

        QString getQName() const;

        const std::string &getName() const;

        const ElementType &getType() const;

        int getAtomicNumber() const;

        void set2D(const float &_x, const float &_y);

        void set3D(const float &_x, const float &_y, const float &_z);

        int getCharge() const;

        float getMass() const;

        float getElectronNeg() const;

        float getRadius() const;
    };

    const std::string &convertElementTypeToName(const ElementType &_type);

    ElementType convertNameToElementType(const std::string &_name);

    QColor getColor(const ElementType &_element);
}
#endif//_XGD_JATOM_HPP_
