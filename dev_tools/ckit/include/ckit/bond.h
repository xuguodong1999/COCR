#pragma once

#include "els_ckit_export.h"
#include "ckit/config.h"
#include "base/bond_type.h"
#include "base/fraction.h"
#include <memory>

namespace v1_0 {
    class JBond;
}

namespace v1_1 {
    class JBond;
}

class ELS_CKIT_EXPORT Bond {
};

class ELS_CKIT_EXPORT GuiBond {
    std::shared_ptr<v1_0::JBond> b;

};

class ELS_CKIT_EXPORT DgBond {
    std::shared_ptr<v1_1::JBond> b;
};