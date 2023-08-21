#pragma once

#include "xgd_chem_export.h"
#include "XGD/Chem/Config.h"
#include <unordered_set>

class XGD_CHEM_EXPORT Ring {
    std::unordered_set<id_type> bondIds;
public:
    Ring();

    void addBond(const id_type &_bid);
};