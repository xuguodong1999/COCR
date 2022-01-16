#ifndef _MOL_HOLDER_HPP_
#define _MOL_HOLDER_HPP_

#include "mol.hpp"

class MolHolder {
protected:
    std::shared_ptr<JMol> mol;
public:
    MolHolder(std::shared_ptr<JMol> _mol);

    std::shared_ptr<JMol> getMol();

    virtual void clear();

    void reset(std::shared_ptr<JMol> _mol);
};

#endif//_MOL_HOLDER_HPP_
