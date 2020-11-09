#ifndef _RDKIT_UTIL_HPP_
#define _RDKIT_UTIL_HPP_

#include <string>

std::string getStandardSMILES(const std::string &smiles,
                              bool add_hydrogen = false, bool do_kekule = false);

#endif//_RDKIT_UTIL_HPP_