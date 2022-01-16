#ifndef _MOL_OP_HPP_
#define _MOL_OP_HPP_

#include "mol_holder.hpp"

class MolOp : public MolHolder {
    std::unordered_map<size_t, frac> atomValenceMap;
    std::unordered_map<size_t, size_t> dbCounter;
    std::unordered_map<size_t, std::unordered_set<size_t>> neighborAtomsMap;
    std::unordered_map<size_t, std::unordered_set<size_t>> neighborBondsMap;
public:
    MolOp(std::shared_ptr<JMol> _mol);

    void addHs(const size_t &_aid);

    bool markAsDouble(const size_t &_bid);

    void clear() override;


private:
    void removeBond(const size_t &_bid);

    void addValence4Atom(const size_t &_aid, const frac &_valence);

    void updateAtomValenceMap();

    void updateAtomValenceMap(const std::vector<std::shared_ptr<JBond>> &_bonds);

    void updateNeighborInfoMap();

    void updateDoubleBondCounter();
};

#endif
