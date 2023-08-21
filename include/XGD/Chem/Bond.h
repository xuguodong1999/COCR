#pragma once

#include "xgd_chem_export.h"
#include "XGD/Chem/Config.h"
#include "XGD/Base/BondType.h"
#include "XGD/Base/Fraction.h"

#include <memory>

class XGD_CHEM_EXPORT Bond {
    BondType bondType;
    id_type atomFrom, atomTo;
    // [0,1], where the bond side access the atom
    float offset1, offset2;
    id_type id;
    std::shared_ptr<Atom> from, to;
public:
    //
    Bond();

    Bond(const id_type &_id, const id_type &_atomFromId, const id_type &_atomToId,
         const BondType &_bondType = BondType::SingleBond);

    Bond(const id_type &_id, std::shared_ptr<Atom> _from = nullptr, std::shared_ptr<Atom> _to = nullptr,
         const BondType &_type = BondType::SingleBond, const float &_offset1 = 0.5, const float &_offset2 = 0.5);

    //
    frac asValence() const;

    //
    const BondType &getBondType() const;

    //
    BondType getType() const;

    void setType(const BondType &_bondType);

    //
    const id_type &getAtomFrom() const;

    const id_type &getAtomTo() const;

    //
    void setId(const id_type &_id);

    id_type getId() const;

    //
    void setFrom(const std::shared_ptr<Atom> &from);

    void setTo(const std::shared_ptr<Atom> &to);

    std::shared_ptr<Atom> getFrom() const;

    std::shared_ptr<Atom> getTo() const;

    //
    bool setFrom(const id_type &_atomFrom);

    void setFromTo(const id_type &_atomFrom, const id_type &_atomTo);

    //
    float getFromOffset() const;

    float getToOffset() const;

    //
    int getBondOrder() const;

    void setOrder(const int &_order);

    //
    std::string getName() const;
};
