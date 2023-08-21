#pragma once

#include "xgd_chem_export.h"
#include "XGD/Chem/Config.h"

class GuiMol;

class XGD_CHEM_EXPORT MolManager {
    gui_mol inputMol, currentMol;

    gui_mol fullHydrogenInputMol;

    gui_mol expandedMol;
    gui_mol fullHydrogenExpandedMol;

    MolManager(gui_mol mol);

    MolManager(MolManager &&) = delete;

    MolManager(const MolManager &) = delete;

public:
    void setInputMol(gui_mol mol);

    gui_mol getInputMol();

    gui_mol getCurrentMol();

    gui_mol getFullHydrogenInputMol();

    gui_mol getExpandedMol();

    gui_mol getFullHydrogenExpandedMol(bool setCurrent = true);

    static MolManager &GetInstance();
};