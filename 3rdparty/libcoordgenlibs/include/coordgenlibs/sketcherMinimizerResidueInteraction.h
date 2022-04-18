/*
 *  sketcherMinimizerResidueInteraction.h
 *
 *  Created by Nicola Zonta on 13/05/2011.
 *   Copyright Schrodinger, LLC. All rights reserved.
 *
 */

#pragma once

#include "coordgenlibs/sketcherMinimizerBond.h"

/* class to represent an interaction with protein residues (e.g. h-bond or
 pi-pi stacking */
class COORDGENLIBS_EXPORT sketcherMinimizerResidueInteraction
        : public sketcherMinimizerBond {
public:
    sketcherMinimizerResidueInteraction();

    ~sketcherMinimizerResidueInteraction() override;

    bool isResidueInteraction() override;

    /* get all the atoms involved at the end side of the interaction */
    std::vector<sketcherMinimizerAtom *> getAllEndAtoms();

    std::vector<sketcherMinimizerAtom *> getAllStartAtoms();

    /* get all the atoms involved at the start side of the interaction */
    std::vector<sketcherMinimizerAtom *> m_otherEndAtoms;
    std::vector<sketcherMinimizerAtom *> m_otherStartAtoms;
};