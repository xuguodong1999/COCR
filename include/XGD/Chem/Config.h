#pragma once

#include "XGD/Base/COCRTypes.h"
#include "XGD/Base/ElementType.h"
#include "XGD/Base/AtomStereo.h"
#include "XGD/Base/BondType.h"
#include <cstddef>
#include <memory>

using id_type = size_t;

class Atom;

class Bond;

class GuiMol;

using gui_mol = std::shared_ptr<GuiMol>;
