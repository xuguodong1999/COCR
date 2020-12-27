#include "mol_base.hpp"

void MolBase::safeTraverseBonds(const std::function<void(const size_t &)> &func) {
    std::vector<size_t> bids;
    for (auto&[id, _]:bondsMap) {
        bids.push_back(id);
    }
    for (auto &bid:bids) {
        func(bid);
    }
}


void MolBase::safeTraverseAtoms(const std::function<void(const size_t &)> &func) {
    std::vector<size_t> aids;
    for (auto&[id, _]:atomsMap) {
        aids.push_back(id);
    }
    for (auto &aid:aids) {
        func(aid);
    }
}