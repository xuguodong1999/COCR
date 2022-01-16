/*
 *  sketcherMinimizerResidue.h
 *
 *  Created by Nicola Zonta on 13/05/2011.
 *   Copyright Schrodinger, LLC. All rights reserved.
 *
 */

#pragma  once

#include "coordgenlibs/sketcherMinimizerAtom.h"
#include "coordgenlibs/sketcherMinimizerBond.h"

/* class to represent protein residues */
class COORDGENLIBS_EXPORT sketcherMinimizerResidue : public sketcherMinimizerAtom {
public:
    sketcherMinimizerResidue();

    ~sketcherMinimizerResidue() override;

    bool isResidue() const override;

    /* compute coordinates based on the position of the closest ligand atom */
    sketcherMinimizerPointF computeStartingCoordinates(float d = 2.f) {
        sketcherMinimizerPointF out = templateCoordinates;
        if (m_closestLigandAtom) {
            out = m_closestLigandAtom->getSingleAdditionVector() * d +
                  m_closestLigandAtom->coordinates;
        }
        if (residueInteractions.size()) {
            int nn = 0;
            sketcherMinimizerPointF coords(0.f, 0.f);
            for (auto &residueInteraction: residueInteractions) {
                sketcherMinimizerAtom *n = residueInteraction->endAtom;
                if (n == this) {
                    n = residueInteraction->startAtom;
                }
                if (!n->isResidue()) {
                    coords += n->getSingleAdditionVector() * d + n->coordinates;
                    nn++;
                } else if (n->coordinatesSet) {
                    coords += n->coordinates;
                    nn++;
                }
            }
            if (nn > 0) {
                coords /= float(nn);
            }
            out = coords;
        }
        return out;
    }

    std::string chain;
    int resnum;
    sketcherMinimizerAtom *m_closestLigandAtom;
};