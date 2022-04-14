#pragma once

 #include "ckit/config.h"
#include "base/fraction.h"
#include "base/bond_type.h"
#include <vector>
#include <string>
#include <unordered_set>
namespace v1_0 {
    class JBond {
        BondType bondType;
        size_t atomFrom, atomTo;
        size_t id;

    public:
        frac asValence() const;

        const BondType &getBondType() const;

        const size_t &getAtomFrom() const;

        const size_t &getAtomTo() const;

        const size_t &getId() const;

        JBond();

        JBond(const size_t &_id, const size_t &_atomFromId, const size_t &_atomToId,
              const BondType &_bondType = BondType::SingleBond);


        void setType(const BondType &_bondType);

        void setFromTo(const size_t &_atomFrom, const size_t &_atomTo);

        bool setFrom(const size_t &_atomFrom);

        void setId(const size_t &_id);
    };

    bool operator==(const JBond &_b1, const JBond &_b2);
}