#include "d_element_valence.h"

// FIXME: add all elements you need here
// H B C N O F Si P S Cl Br I
const std::unordered_map<ElementType, frac> ElementValenceData{
        {ElementType::H,  1},
        {ElementType::He, 2},
        {ElementType::B,  3},
        {ElementType::C,  4},
        {ElementType::N,  3},
        {ElementType::O,  2},
        {ElementType::F,  1},
        {ElementType::Si, 4},
        {ElementType::P,  3},
        {ElementType::S,  2},
        {ElementType::Cl, 1},
        {ElementType::Br, 1},
        {ElementType::I,  1}
};
