#ifndef _BOND_HPP_
#define _BOND_HPP_

#include "config.hpp"

enum class JBondType {
    SingleBond,
    SolidWedgeBond,
    DashWedgeBond,
    WaveBond,

    DoubleBond,
    TripleBond,

    DelocalizedBond,
};

class JBond {
    size_t id;
    JBondType bondType;
    size_t atomFrom,atomTo;
public:
    JBond(const size_t&_atomFromId,const size_t&_atomToId);
};

class JRing{
    size_t id;
    std::vector<size_t> atomIds;
public:
    JRing();
};
#endif//_BOND_HPP_