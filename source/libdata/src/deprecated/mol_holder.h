#pragma once

#include "ckit/config.h"
#include "mol.h"

namespace data_deprecated {
    class MolHolder {
    protected:
        std::shared_ptr<JMol> mol;
    public:
        MolHolder(std::shared_ptr<JMol> _mol);

        std::shared_ptr<JMol> getMol();

        virtual void clear();

        void reset(std::shared_ptr<JMol> _mol);

    };
}