#include "jmol_manager.hpp"
#include "jmol_adapter.hpp"

using namespace xgd;

JMolManager::JMolManager(JMolManager::mol_type _inputMol) :
        inputMol(std::move(_inputMol)), expandedMol(nullptr),
        fullHydrogenExpandedMol(nullptr), fullHydrogenInputMol(nullptr) {

}

void JMolManager::setInputMol(JMolManager::mol_type _inputMol) {
    inputMol = std::move(_inputMol);
    expandedMol = fullHydrogenExpandedMol = fullHydrogenInputMol = nullptr;
}

JMolManager::mol_type JMolManager::getInputMol() {
    return inputMol;
}

JMolManager::mol_type JMolManager::getFullHydrogenInputMol() {
    if (!fullHydrogenInputMol) {
        fullHydrogenInputMol = inputMol->deepClone();
        fullHydrogenInputMol->addAllHydrogens();
    }
    return fullHydrogenInputMol;
}


JMolManager::mol_type JMolManager::getFullHydrogenExpandedMol() {
    if (!fullHydrogenExpandedMol) {
        fullHydrogenExpandedMol = getExpandedMol()->deepClone();
        fullHydrogenExpandedMol->addAllHydrogens();
    }
    return fullHydrogenExpandedMol;
}

JMolManager::mol_type JMolManager::getExpandedMol() {
    if (!expandedMol) {
        expandedMol = inputMol->deepClone();
        expandedMol->tryExpand();
    }
    return expandedMol;
}
