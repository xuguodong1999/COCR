#include "mol_holder.hpp"


std::shared_ptr<JMol> MolHolder::getMol() {
    return mol;
}

MolHolder::MolHolder(std::shared_ptr<JMol> _mol) : mol(std::move(_mol)) {

}

void MolHolder::clear() {
    mol = nullptr;
}

void MolHolder::reset(std::shared_ptr<JMol> _mol) {
    mol = std::move(_mol);
}
