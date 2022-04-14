#pragma once

#include "els_ckit_export.h"
#include "ckit/config.h"
#include "base/fraction.h"
#include "base/point2.h"
#include "base/point3.h"
#include "base/element_type.h"
#include "base/bond_type.h"
#include <unordered_set>
#include <unordered_map>
#include <vector>
#include <functional>

namespace v1_0 {
    class JMol;
}

namespace v1_1 {
    class JMol;
}

class ELS_CKIT_EXPORT Mol {

};

class ELS_CKIT_EXPORT GuiMol {
    std::shared_ptr<v1_0::JMol> m;
};

class ELS_CKIT_EXPORT DgMol {
    std::shared_ptr<v1_1::JMol> m;
};