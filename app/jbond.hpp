#ifndef _JBOND_HPP_
#define _JBOND_HPP_

#include <memory>
#include <QColor>

namespace xgd {
    class JAtom;

    enum class BondType : int {
        ImplicitBond = 0,
        SingleBond = 1,
        DoubleBond = 2,
        TripleBond = 3,
        UpBond = 4,
        DownBond = 5,
        DelocalizedBond = 6
    };

    class JBond {
        std::shared_ptr<JAtom> from, to;
        size_t id;
        BondType type;

    public:
        std::shared_ptr<JAtom> getFrom() const;

        std::shared_ptr<JAtom> getTo() const;

        size_t getId() const;

        BondType getType() const;

        int getBondOrder() const;

        void setType(const BondType &_type);

        void setOrder(const int &_order);

        size_t fromId() const;

        size_t toId() const;

        JBond(const size_t &_id, std::shared_ptr<JAtom> _from = nullptr, std::shared_ptr<JAtom> _to = nullptr,
              const BondType &_type = BondType::SingleBond);
    };

    QColor getColor(const BondType &_bondType);
}
#endif//_JBOND_HPP_
