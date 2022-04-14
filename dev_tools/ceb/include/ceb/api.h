#pragma once

#include "els_ceb_export.h"
#include <vector>
#include <optional>

class ELS_CEB_EXPORT CebApi {
public:
    static std::optional<std::vector<int>> balance(const std::string &equation);
    static std::optional<std::string> balanceAndRenderToHtml(const std::string &equation);
};