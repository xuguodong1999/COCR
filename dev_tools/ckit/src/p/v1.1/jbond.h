#pragma once

 #include "ckit/config.h"
#include "base/cocr_types.h"

#include <string>
#include <memory>


namespace v1_1 {
    class JAtom;

//    enum class BondType : int {
//        ImplicitBond = 0,
//        SingleBond = 1,
//        DoubleBond = 2,
//        TripleBond = 3,
//        UpBond = 4,
//        DownBond = 5,
//        DelocalizedBond = 6
//    };

    class JBond {
        // 约定：范围[0,1]，表达键端在原子的接入点，用于几何表达层面
        float offset1, offset2;
        BondType type;
        id_type id;
        std::shared_ptr<JAtom> from, to;
    public:
        void setFrom(const std::shared_ptr<JAtom> &from);

        void setTo(const std::shared_ptr<JAtom> &to);

        std::shared_ptr<JAtom> getFrom() const;

        std::shared_ptr<JAtom> getTo() const;

        float getFromOffset() const;

        float getToOffset() const;

        id_type getId() const;

        BondType getType() const;

        int getBondOrder() const;

        void setType(const BondType &_type);

        void setOrder(const int &_order);

        std::string getName() const;

        JBond(const id_type &_id, std::shared_ptr<JAtom> _from = nullptr, std::shared_ptr<JAtom> _to = nullptr,
              const BondType &_type = BondType::SingleBond, const float &_offset1 = 0.5, const float &_offset2 = 0.5);
    };

    ColorName getBondColor(const BondType &_bondType);
}