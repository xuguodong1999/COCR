#pragma once
#include <memory>

namespace cocr {
    class JMol;

    class JMolManager {
        using mol_type = std::shared_ptr<JMol>;
        mol_type inputMol, currentMol;

        mol_type fullHydrogenInputMol;

        mol_type expandedMol;
        mol_type fullHydrogenExpandedMol;

        JMolManager(mol_type _inputMol);

        JMolManager(JMolManager &&) = delete;

        JMolManager(const JMolManager &) = delete;

    public:
        void setInputMol(mol_type _inputMol);

        mol_type getInputMol();

        mol_type getCurrentMol();

        mol_type getFullHydrogenInputMol();

        mol_type getExpandedMol();

        mol_type getFullHydrogenExpandedMol(bool _set_current = true);

        static JMolManager &GetInstance();
    };
}