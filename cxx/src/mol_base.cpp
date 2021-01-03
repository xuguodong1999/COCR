#include "mol_base.hpp"


const std::shared_ptr<JMol> &MolBase::getOldMol() const {
    return oldMol;
}

const std::shared_ptr<JMol> &MolBase::getNewMol() const {
    return newMol;
}

MolBase::MolBase(std::shared_ptr<JMol> _mol) :oldMol(std::move(_mol)),newMol(nullptr){

}

MolBase::MolBase(std::shared_ptr<JMol> _oldMol, std::shared_ptr<JMol> _newMol)
        :oldMol(std::move(_oldMol)),newMol(_newMol){

}
