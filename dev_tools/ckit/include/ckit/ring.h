#pragma once

#include "els_ckit_export.h"
#include "ckit/config.h"
#include <unordered_set>

class ELS_CKIT_EXPORT Ring {
    std::unordered_set<id_type> bondIds;
public:
    Ring();

    void addBond(const id_type &_bid);
};