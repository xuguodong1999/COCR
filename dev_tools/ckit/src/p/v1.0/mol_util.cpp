#include "mol_util.h"
#include "math/alkane_graph.h"
#include "base/std_util.h"

#include "openbabel_util.h"
using namespace v1_0;
std::shared_ptr<MolUtil> molUtil;

std::vector<std::shared_ptr<JMol>> MolUtil::split(const JMol &_mol) {
    AlkaneGraph<size_t> graph;
    std::unordered_map<size_t, size_t> j2gAidMap, g2jMap;
    std::vector<std::shared_ptr<JMol>> result;
    size_t gAidIdx = 0;
    _mol.safeTraverseAtoms([&](const size_t &aid) { j2gAidMap[aid] = gAidIdx++; });
    _mol.safeTraverseBonds([&](const size_t &bid) {
        auto bond = _mol.getBondById(bid);
        if (!bond)return;
        graph.push_back(j2gAidMap[bond->getAtomFrom()], j2gAidMap[bond->getAtomTo()]);
    });
    for (auto&[jAid, gAid]: j2gAidMap) g2jMap[gAid] = jAid;
    // 保证一个连通片不和其它连通片共享边 or 节点
    std::vector<std::unordered_set<size_t>> groups = graph.dfsTraceGroup();
    for (auto &group: groups) {
        auto mol = std::make_shared<JMol>();
        std::unordered_map<size_t, size_t> aidMap;//<old,new>
        for (auto &gid: group) {
            size_t oldAid = g2jMap[gid];
            auto atom = _mol.getAtomById(oldAid);
            if (!atom) continue;
            auto newAtom = mol->addAtom(atom->getAtomicNumber());
            aidMap[oldAid] = newAtom->getId();
        }
        _mol.safeTraverseBonds([&](const size_t &bid) {
            auto bond = _mol.getBondById(bid);
            if (!bond)return;
            if (StdUtil::notExist(group, bond->getAtomFrom())) return;
            mol->addBond(aidMap[bond->getAtomFrom()], aidMap[bond->getAtomTo()],
                         bond->getBondType());
        });
        result.push_back(std::move(mol));
    }
    return std::move(result);
}
