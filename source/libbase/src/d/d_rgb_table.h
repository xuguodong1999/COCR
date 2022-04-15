#pragma once

#include "base/color_name.h"
#include "base/bond_type.h"
#include "base/element_type.h"
#include <unordered_map>

extern const unsigned char predefinedColors[][3];

extern const std::unordered_map<BondType, ColorName> bondColorMap;

extern const std::unordered_map<ElementType, ColorName> atomColorMap;