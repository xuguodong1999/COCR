#include "XGD/Chem/MolManager.h"
#include "XGD/Chem/Mol.h"

MolManager::MolManager(gui_mol mol)
        : inputMol(mol), currentMol(mol), expandedMol(nullptr),
          fullHydrogenExpandedMol(nullptr), fullHydrogenInputMol(nullptr) {

}

void MolManager::setInputMol(gui_mol mol) {
    currentMol = inputMol = std::move(mol);
    expandedMol = fullHydrogenExpandedMol = fullHydrogenInputMol = nullptr;
}

gui_mol MolManager::getInputMol() {
    currentMol = inputMol;
    return inputMol;
}

gui_mol MolManager::getCurrentMol() {
    if (!currentMol) {
        currentMol = inputMol;
    }
    return currentMol;
}

gui_mol MolManager::getFullHydrogenInputMol() {
    if (!fullHydrogenInputMol) {
        fullHydrogenInputMol = getInputMol()->deepClone();
        fullHydrogenInputMol->addAllHydrogens();
    }
    currentMol = fullHydrogenInputMol;
    return fullHydrogenInputMol;
}

gui_mol MolManager::getExpandedMol() {
    if (!expandedMol) {
        expandedMol = inputMol->deepClone();
        expandedMol->tryExpand();
    }
    currentMol = expandedMol;
    return expandedMol;
}

gui_mol MolManager::getFullHydrogenExpandedMol(bool setCurrent) {
    if (!fullHydrogenExpandedMol) {
        fullHydrogenExpandedMol = getExpandedMol()->deepClone();
        fullHydrogenExpandedMol->addAllHydrogens();
    }
    if (setCurrent) {
        currentMol = fullHydrogenExpandedMol;
    }
    return fullHydrogenExpandedMol;
}

MolManager &MolManager::GetInstance() {
    static MolManager manager(nullptr);
    return manager;
}
