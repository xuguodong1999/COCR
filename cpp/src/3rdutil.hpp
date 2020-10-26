#ifndef _3RDUTIL_HPP_
#define _3RDUTIL_HPP_
#include "config.hpp"
#include <string>

std::string getStandardSMILES(const std::string&smiles,bool add_hydrogen=false,bool do_kekule=false);

#endif//_3RDUTIL_HPP_