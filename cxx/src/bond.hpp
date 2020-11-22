#ifndef _BOND_HPP_
#define _BOND_HPP_
#include "fraction.hpp"
#include <vector>
#include <string>
#include <unordered_set>
enum class JBondType {
    SingleBond = 1,// 单键，常规
    DoubleBond = 2,// 双键
    TripleBond = 3,// 三键

    SolidWedgeBond, // 单键，实楔形
    DashWedgeBond,  // 单键，虚楔形
    WaveBond,       // 单键，波浪线

    DelocalizedBond,// 离域键
};
class JBond {
    JBondType bondType;
    size_t atomFrom, atomTo;
    size_t id;
public:
    frac asValence()const;
    const JBondType& getBondType() const;

    const size_t& getAtomFrom() const;

    const size_t& getAtomTo() const;

    const size_t& getId()const;

    JBond();

    JBond(const size_t &_id, const size_t &_atomFromId, const size_t &_atomToId,
          const JBondType &_bondType = JBondType::SingleBond);

    friend bool operator==(const JBond &_b1, const JBond &_b2) ;

    void setType(const JBondType &_bondType);

    void setFromTo(const size_t &_atomFrom, const size_t &_atomTo);

    void setId(const size_t&_id);
};

class JRing {
    std::unordered_set<size_t> bondIds;
public:
    JRing();
    void addBond(const size_t&_bid);
};
//bool operator==(const JBond &_b1, const JBond &_b2);
#endif//_BOND_HPP_