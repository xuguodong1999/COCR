#pragma once

#include "base/cocr_types.h"
#include "base/element_type.h"
#include "base/atom_stereo.h"
#include "base/bond_type.h"
#include <cstddef>
#include <memory>

using id_type = size_t;

class Atom;

class Bond;

class GuiMol;

using gui_mol = std::shared_ptr<GuiMol>;