#ifndef _XGD_EXPAND_UTIL_
#define _XGD_EXPAND_UTIL_

#include "jmol.hpp"
#include <unordered_map>
#include <memory>

namespace xgd {
    class JAtom;

    extern std::unordered_map<std::string, int> SUPER_ATOM_MAP;

    bool tryExpand(JMol &_mol, std::shared_ptr<JAtom> _atom);
}
#endif//_XGD_EXPAND_UTIL_
