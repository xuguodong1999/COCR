#pragma once

#include "els_ckit_export.h"
#include "ckit/config.h"

namespace v1_1 {
    class JMolManager;
}

class ELS_CKIT_EXPORT MolManager {
    std::shared_ptr<v1_1::JMolManager> manager;
};