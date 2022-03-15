#pragma once

///
// Shim for creating sketcherMolecules from .mae files.
#include "coordgenlibs/sketcherMinimizerMolecule.h"
#include "maeparser/Reader.hpp"

///
// A very simple utility function to parse a mae::Block into a 2D
// sketcherMinimizerMolecule. Anything beyond atomic number, x and y coordinates
// and bond orders will be ignored (i.e. no chiralities or stereo bonds will be
// parsed).
//
sketcherMinimizerMolecule * mol_from_mae_block(schrodinger::mae::Block &block);