#include "xgd_chem_export.h"

#include <string>

namespace xgd::chem {
    std::string XGD_CHEM_EXPORT convert(
            const std::string &input,
            const std::string &inputFormat,
            const std::string &outputFormat,
            const bool &gen2D,
            const bool &gen3D
    );
}