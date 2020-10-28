#ifndef _MOLE_HPP_
#define _MOLE_HPP_

#include "config.hpp"
#include "atom.hpp"
#include "bond.hpp"
#include <map>

class JMol :public IdInterface{
    std::map<size_t, std::shared_ptr<JBond>> bondsMap;
    std::map<size_t, std::shared_ptr<JAtom>> atomsMap;

public:
    JMol(const std::string &_smiles);

    void set(const std::string &_smiles);

    void clear();

    std::shared_ptr<JAtom> addAtom(const size_t &_atomicNumber = static_cast<size_t>(ElementType::C));

    std::shared_ptr<JBond>
    addBond(const size_t &_atomFromId, const size_t &_atomToId);

    std::shared_ptr<JBond> JMol::addBond(const size_t &_atomFromId, const size_t &_atomToId, const JBondType &_bondType);

};

#endif//_MOLE_HPP_