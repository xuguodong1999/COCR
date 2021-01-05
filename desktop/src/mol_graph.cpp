#include "mol_graph.hpp"
#include "mol_util.hpp"


std::vector<std::shared_ptr<JMol>> MolGraph::iSplit() const {
    return split(mol);
}

MolGraph::MolGraph(std::shared_ptr<JMol> _mol): MolHolder(std::move(_mol)) {

}



const std::vector<std::shared_ptr<JRing>>& MolGraph::calcLSSR(
        const std::shared_ptr<JMol> &_mol, bool _isDirNeeded) {
    return rings;
}
