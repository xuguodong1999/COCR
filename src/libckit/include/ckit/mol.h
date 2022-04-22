#pragma once

#include "els_ckit_export.h"
#include "ckit/config.h"
#include "base/fraction.h"
#include "base/point2.h"
#include "base/point3.h"
#include "base/element_type.h"
#include "base/bond_type.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>

namespace v1_0 {
    class JMol;
}

namespace ckit_deprecated {
    class JMol;
}

class ELS_CKIT_EXPORT Mol {

};

class ELS_CKIT_EXPORT GuiMol {
    std::shared_ptr<ckit_deprecated::JMol> m;
public:
    GuiMol();

    GuiMol(GuiMol &&mol) noexcept;

    GuiMol(const GuiMol &mol);

    gui_mol deepClone() const;

    void addAllHydrogens();

    bool tryExpand();

    void set2DInfoLatest(bool is2DInfoLatest = true);

    void norm2D(
            const float &w, const float &h,
            const float &x = 0, const float &y = 0,
            bool keepRatio = true);

    void norm3D(
            const float &xx, const float &yy, const float &zz,
            const float &x = 0, const float &y = 0, const float &z = 0,
            bool keepRatio = true);

    float getAvgBondLength();

    void loopAtomVec(std::function<void(Atom &atom)> func);

    void loopBondVec(std::function<void(Bond &bond)> func);

    std::shared_ptr<Atom> getAtom(const id_type &aid);

    std::shared_ptr<Bond> getBond(const id_type &bid);

    std::shared_ptr<Atom> addAtom(
            const ElementType &element, const float &x, const float &y, const float &z = 0);

    std::shared_ptr<Bond> addBond(
            std::shared_ptr<Atom> a1, std::shared_ptr<Atom> a2,
            const BondType &type = BondType::SingleBond,
            const float &offset1 = 0.5, const float &offset2 = 0.5);

    std::string writeAs(const std::string &suffix);

    std::shared_ptr<Atom> addSuperAtom(
            const std::string &name,
            const float &x0 = 0, const float &y0 = 0,
            const float &x1 = 0, const float &y1 = 0);

    std::vector<std::vector<id_type>> getSSSR();

    void tryMarkDoubleBond(const id_type &bid);
};