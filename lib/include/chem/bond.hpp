#pragma once

#include <cocr_chem_export.h>
#include "base/fraction.hpp"
#include "chem/bond_type.hpp"
#include <vector>
#include <string>
#include <unordered_set>

class COCR_CHEM_EXPORT JBond {
    JBondType bondType;
    size_t atomFrom, atomTo;
    size_t id;

public:
    frac asValence() const;

    const JBondType &getBondType() const;

    const size_t &getAtomFrom() const;

    const size_t &getAtomTo() const;

    const size_t &getId() const;

    JBond();

    JBond(const size_t &_id, const size_t &_atomFromId, const size_t &_atomToId,
          const JBondType &_bondType = JBondType::SingleBond);

    friend bool operator==(const JBond &_b1, const JBond &_b2);

    void setType(const JBondType &_bondType);

    void setFromTo(const size_t &_atomFrom, const size_t &_atomTo);

    bool setFrom(const size_t &_atomFrom);

    void setId(const size_t &_id);
};

class JRing {
    std::unordered_set<size_t> bondIds;
public:
    JRing();

    void addBond(const size_t &_bid);
};
//bool operator==(const JBond &_b1, const JBond &_b2);