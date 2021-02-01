#ifndef _MOL_HPP_
#define _MOL_HPP_

#include "atom.hpp"
#include "bond.hpp"
#include <memory>
#include <functional>
#include <unordered_map>

class JMol {
    size_t mAids, mBids;
    std::unordered_map<size_t, std::shared_ptr<JAtom>> atomsMap;
    std::unordered_map<size_t, std::shared_ptr<JBond>> bondsMap;
public:
    JMol(const JMol &_mol);

    JMol(JMol &&_mol);

    JMol();

    std::shared_ptr<JMol> clone() const;

    void setAlkane(const std::string &_alkaneSMILES);

    std::shared_ptr<JAtom> getAtomById(const size_t &_aid) const;

    std::shared_ptr<JBond> getBondById(const size_t &_bid) const;

    bool IsBondsEmpty() const;

    size_t bondsNum() const;

    size_t atomsNum() const;

    void safeTraverseBonds(const std::function<void(const size_t &)> &func) const;

    void safeTraverseBondsBreakIf(const std::function<bool(const size_t &)> &func) const;

    void safeTraverseAtoms(const std::function<void(const size_t &)> &func) const;

    void safeTraverseAtomsBreakIf(const std::function<bool(const size_t &)> &func) const;

    virtual void clear();

    void removeBond(const size_t &_bid);

    std::shared_ptr<JAtom> addAtom(const size_t &_atomicNumber);

    std::shared_ptr<JAtom> addAtom(const ElementType &_elementType);

    std::shared_ptr<JBond> addBond(const size_t &_atomFromId, const size_t &_atomToId,
                                   const JBondType &_bondType = JBondType::SingleBond);

private:
};

#endif