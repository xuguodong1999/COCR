#pragma once

#include "els_ckit_export.h"
#include "ckit/config.h"

class GuiMol;

class ELS_CKIT_EXPORT MolManager {
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