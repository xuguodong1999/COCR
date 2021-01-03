#ifndef _MOL_BASE_HPP_
#define _MOL_BASE_HPP_

#include "mol.hpp"
class MolBase{
protected:
    std::shared_ptr<JMol>oldMol,newMol;
public:
    MolBase(std::shared_ptr<JMol> _mol);
    MolBase(std::shared_ptr<JMol> _oldMol,std::shared_ptr<JMol> _newMol);

    const std::shared_ptr<JMol> &getOldMol() const;

    const std::shared_ptr<JMol> &getNewMol() const;


};
#endif//_MOL_BASE_HPP_
