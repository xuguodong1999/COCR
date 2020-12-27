#ifndef _RDKIT_UTIL_HPP_
#define _RDKIT_UTIL_HPP_

#include "mol.hpp"

#include <string>

//std::string getStandardSMILES(const std::string &smiles,
//                              bool add_hydrogen = false, bool do_kekule = false);

std::string getSMILES_openbabelImpl(const JMol&_mol);
#endif//_RDKIT_UTIL_HPP_