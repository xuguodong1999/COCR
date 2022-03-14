/*
 * A very, very basic SMILES-like parser. No aromaticity,
 * or zero-order bonds. No chirality/stereochemistry
 *
 * Do not use this as a basis for real SMILES parsers. It just lets
 * us sidestep using a full chemistry toolkit when writing tests.
 */
#pragma once

#include <string>

class sketcherMinimizerMolecule;

namespace schrodinger {
    sketcherMinimizerMolecule *approxSmilesParse(const std::string &smiles);

} // namespace schrodinger
