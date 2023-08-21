#pragma once

#include "xgd_chem_export.h"
#include <vector>
#include <optional>
#include <string>

class XGD_CHEM_EXPORT CebApi {
public:
    static std::optional<std::vector<int>> balance(const std::string &equation);

    static std::optional<std::string> balanceAndRenderToHtml(const std::string &equation);
};