#include "MolGraph.h"
#include "MolUtil.h"

using namespace data_deprecated;
extern std::shared_ptr<MolUtil> molUtil;

std::vector<std::shared_ptr<JMol>> MolGraph::iSplit() const {
    return molUtil->split(*mol);
}

MolGraph::MolGraph(std::shared_ptr<JMol> _mol) : MolHolder(std::move(_mol)) {

}


const std::vector<std::shared_ptr<Ring>> &MolGraph::calcLSSR(
        const std::shared_ptr<JMol> &_mol, bool _isDirNeeded) {
    return rings;
}
