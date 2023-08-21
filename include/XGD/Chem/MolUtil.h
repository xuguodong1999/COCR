#pragma once

#include "xgd_chem_export.h"
#include "XGD/Chem/Config.h"

class XGD_CHEM_EXPORT MolUtil {
public:
    static bool IsValidWritableFormat(const std::string &suffix);

    // define it in specific impl
    static std::vector<std::string> GetWritableFormats();
};