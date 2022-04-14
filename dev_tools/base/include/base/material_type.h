#pragma once

#include "els_base_export.h"
#include "base/fraction.h"
#include <unordered_map>
#include <vector>
#include <optional>

class ELS_BASE_EXPORT MaterialUtil {
public:
    static bool Exist(const std::string &name);

    static const std::vector<frac> &QueryValence(const std::string &name);

    static const std::string &QueryStateInfo(const std::string &name);
};