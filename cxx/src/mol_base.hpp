#ifndef _MOL_BASE_HPP_
#define _MOL_BASE_HPP_

#include "atom.hpp"
#include "bond.hpp"

#include <unordered_map>
#include <memory>

class MolBase {
    // 维护原子和键的自增 id
    size_t mAids, mBids;
    //<原子id，原子>
    std::unordered_map<size_t, std::shared_ptr<JAtom>> atomsMap;
    //<键id，键>
    std::unordered_map<size_t, std::shared_ptr<JBond>> bondsMap;
protected:

public:
    MolBase();

    MolBase(const MolBase &_mol);

    MolBase(MolBase &&_mol);
};

#endif//_MOL_BASE_HPP_
