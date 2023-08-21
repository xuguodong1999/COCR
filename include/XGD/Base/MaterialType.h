#pragma once

#include "xgd_base_export.h"
#include "XGD/Base/Fraction.h"
#include <unordered_map>
#include <vector>
#include <optional>

class XGD_BASE_EXPORT MaterialUtil {
public:
    static bool Exist(const std::string &name);

    static const std::vector<frac> &QueryValence(const std::string &name);

    static const std::string &QueryStateInfo(const std::string &name);
};