#ifndef _BOND_HPP_
#define _BOND_HPP_

#include "config.hpp"
#include <set>

enum class JBondType {
    SingleBond = 1,
    DoubleBond = 2,
    TripleBond = 3,

    SolidWedgeBond,
    DashWedgeBond,
    WaveBond,

    DelocalizedBond,
};
enum class JStereoType {
    // 朝向
    Wedge,
    Hash,
    Wave,
    // 顺反
    Trans,
    Cis,
    Normal
};

class JBond : public IdInterface {
    JBondType bondType;
    size_t atomFrom, atomTo;
public:
    JBondType getBondType() const;


    size_t getAtomFrom() const;

    size_t getAtomTo() const;

public:
    JBond(const size_t &_atomFromId, const size_t &_atomToId);

    friend bool operator==(const JBond &_b1, const JBond &_b2) {
        return _b1.atomTo == _b2.atomTo && _b2.atomFrom == _b2.atomFrom;
    }

    void setType(const JBondType &_bondType = JBondType::SingleBond);
};

class JRing {
    size_t id;
    std::set<size_t> atomIds;
    std::vector<size_t> bondIds;
public:
    JRing();
};

#endif//_BOND_HPP_