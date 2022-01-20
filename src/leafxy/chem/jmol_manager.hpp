#ifndef _XGD_JMOL_MANAGER_HPP_
#define _XGD_JMOL_MANAGER_HPP_

#include <memory>

namespace xgd {
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
#endif//_XGD_JMOL_MANAGER_HPP_
