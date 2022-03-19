#pragma once
#include <cocr_chem_export.h>
#include "chem/mol.hpp"

class COCR_CHEM_EXPORT MolHolder {
protected:
    std::shared_ptr<JMol> mol;
public:
    MolHolder(std::shared_ptr<JMol> _mol);

    std::shared_ptr<JMol> getMol();

    virtual void clear();

    void reset(std::shared_ptr<JMol> _mol);

};