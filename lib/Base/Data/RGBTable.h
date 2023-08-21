#pragma once

#include "XGD/Base/ColorName.h"
#include "XGD/Base/BondType.h"
#include "XGD/Base/ElementType.h"
#include <unordered_map>

extern const unsigned char predefinedColors[][3];

extern const std::unordered_map<BondType, ColorName> bondColorMap;

extern const std::unordered_map<ElementType, ColorName> atomColorMap;