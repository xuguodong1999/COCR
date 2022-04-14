#pragma once

#include "els_ckit_export.h"
#include "ckit/config.h"
#include "base/element_type.h"
#include "base/atom_stereo.h"

#include <vector>
#include <memory>

namespace v1_0 {
    class JAtom;
}
namespace v1_1 {
    class JAtom;
}
class ELS_CKIT_EXPORT Atom {

};

class ELS_CKIT_EXPORT GuiAtom {
    std::shared_ptr<v1_0::JAtom> a;

};

class ELS_CKIT_EXPORT DgAtom {
    std::shared_ptr<v1_1::JAtom> a;
};