#include "ElementNeb.h"

const std::unordered_map<ElementType, int> ELEMENT_COMMON_NEB_NUM_MAP = {
        {ElementType::H,  1},
        {ElementType::Li, 1},
        {ElementType::Na, 1},
        {ElementType::K,  1},
        {ElementType::Rb, 1},
        {ElementType::Sr, 1},
        {ElementType::Fr, 1},

        {ElementType::F,  1},
        {ElementType::Cl, 1},
        {ElementType::Br, 1},
        {ElementType::I,  1},

        {ElementType::C,  4},
        {ElementType::Si, 4},

        {ElementType::B,  3},
        {ElementType::Al, 3},
        {ElementType::Ga, 3},
        {ElementType::In, 3},
        {ElementType::Ta, 3},

        {ElementType::N,  3},
        {ElementType::P,  3},
        {ElementType::As, 3},
        {ElementType::Sb, 3},

        {ElementType::Bi, 5},

        {ElementType::O,  2},
        {ElementType::S,  2},
        {ElementType::Se, 2},

        {ElementType::Mg, 2},
        {ElementType::Al, 2}
};