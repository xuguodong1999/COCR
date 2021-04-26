#ifndef _XGD_EXPAND_UTIL_
#define _XGD_EXPAND_UTIL_

#include "jmol.hpp"
#include <unordered_map>
#include <memory>

namespace xgd {
    class JAtom;

    enum class AbbType {
        Number,
        Element,
        Single,
        Double,
        Triple,
        Neg,
        Pos,
        Left,
        Right,


        Me,
        Et,
        iPr,
        nPr, Pr,
        SO2, SOO,
        CO2, COO,
        CSO,
        COS,
        // 所有元素符号

    };
    extern std::unordered_map<std::string, AbbType> SUPER_ATOM_MAP;

    void initSuperAtomMap();

    bool tryExpand(xgd::JMol &mol, std::shared_ptr<JAtom> atom);
}
#endif//_XGD_EXPAND_UTIL_
