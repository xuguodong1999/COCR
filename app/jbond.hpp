#ifndef _XGD_JBOND_HPP_
#define _XGD_JBOND_HPP_

#include <memory>
#include <QColor>
#include <QString>
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
        size_t id;
        std::shared_ptr<JAtom> from, to;
        // 约定：范围[0,1]，表达键端在原子的接入点，用于几何表达层面
        float offset1, offset2;
        BondType type;
    public:
        std::shared_ptr<JAtom> getFrom() const;

        std::shared_ptr<JAtom> getTo() const;

        float getFromOffset() const;

        float getToOffset() const;

        size_t getId() const;

        BondType getType() const;

        QString getQName()const;

        int getBondOrder() const;

        void setType(const BondType &_type);

        void setOrder(const int &_order);

        JBond(const size_t &_id, std::shared_ptr<JAtom> _from = nullptr, std::shared_ptr<JAtom> _to = nullptr,
              const BondType &_type = BondType::SingleBond, const float &_offset1 = 0.5, const float &_offset2 = 0.5);
    };

    QColor getColor(const BondType &_bondType);
}
#endif//_XGD_JBOND_HPP_
