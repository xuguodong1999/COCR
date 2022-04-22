#pragma once

#include "els_ckit_export.h"
#include "ckit/config.h"

class ELS_CKIT_EXPORT MolUtil {
public:
    static bool IsValidWritableFormat(const std::string &suffix);

    // define it in specific impl
    static std::vector<std::string> GetWritableFormats();
};