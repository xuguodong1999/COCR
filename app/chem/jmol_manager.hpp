#ifndef _XGD_JMOL_MANAGER_HPP_
#define _XGD_JMOL_MANAGER_HPP_

#include <memory>

namespace xgd {
    class JMol;

    class JMolManager {
        using mol_type = std::shared_ptr<JMol>;
        mol_type inputMol;

        mol_type fullHydrogenInputMol;

        mol_type expandedMol;
        mol_type fullHydrogenExpandedMol;
    public:
        JMolManager(mol_type _inputMol);

        void setInputMol(mol_type _inputMol);

        mol_type getInputMol();

        mol_type getFullHydrogenInputMol();

        mol_type getExpandedMol();

        mol_type getFullHydrogenExpandedMol();

    };
}
#endif//_XGD_JMOL_MANAGER_HPP_
