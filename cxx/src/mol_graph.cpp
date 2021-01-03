#include "mol_graph.hpp"
#include "mol_util.hpp"


std::vector<std::shared_ptr<JMol>> MolGraph::iSplit() const {
    return split(oldMol);
}

MolGraph::MolGraph(std::shared_ptr<JMol> _mol):MolBase(std::move(_mol)) {

}

const std::shared_ptr<JMol> &MolGraph::getOldMol() const {
    return oldMol;
}

const std::shared_ptr<JMol> &MolGraph::getNewMol() const {
    return newMol;
}

const std::vector<std::shared_ptr<JRing>>& MolGraph::calcLSSR(
        const std::shared_ptr<JMol> &_mol, bool _isDirNeeded) {
    return rings;
}
