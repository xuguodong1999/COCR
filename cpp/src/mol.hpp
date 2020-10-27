#ifndef _MOLE_HPP_
#define _MOLE_HPP_
#include "config.hpp"
#include "atom.hpp"
#include "bond.hpp"
class JMol {
    std::vector<std::shared_ptr<JBond>> bonds;
    std::vector<std::shared_ptr<JAtom>> atoms;
};
#endif//_MOLE_HPP_