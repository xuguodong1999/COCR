#pragma once

#include "els_ceb_export.h"
#include "base/fraction.h"
#include <vector>
#include <optional>

class ELS_CEB_EXPORT CebApi {
public:
    static std::optional<std::vector<frac>> balance(const std::string &equation);
    static std::string balanceAndRenderToHtml(const std::string &equation);
};