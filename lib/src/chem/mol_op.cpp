#include "chem/mol_op.hpp"
#include "base/std_util.hpp"
#include <random>


void MolOp::clear() {
    MolHolder::clear();
    atomValenceMap.clear();
    neighborAtomsMap.clear();
    neighborBondsMap.clear();
    aromaticRingAids.clear();
    aromaticRingBids.clear();
}

void MolOp::randomize(
        const float &_addHydrogenProb, bool _replaceBond,
        bool _replaceAtom, bool _addAromaticRing, bool _addCommonRing) {
    updateAtomValenceMap();
    // 换化学键类型
    auto replace_line_bond_type = [&](const size_t &_bid) {
        const auto &bond = mol->getBondById(_bid);
        const size_t &from = bond->getAtomFrom(), to = bond->getAtomTo();
        const auto &fromAtom = mol->getAtomById(from);
        const auto &toAtom = mol->getAtomById(to);
        const frac deltaFrom = ElementValenceData[fromAtom->getElementType()]
                               - atomValenceMap[from];
        const frac deltaTo = ElementValenceData[toAtom->getElementType()]
                             - atomValenceMap[to];
        if (deltaFrom >= 1 && deltaTo >= 1) {
            frac delta = std::min(deltaFrom, deltaTo);
            std::vector<JBondType> sBondCandidates;
            if (delta >= 2) {// 三键、双键
                sBondCandidates = {JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::SingleBond, JBondType::WaveBond,
                                   JBondType::DoubleBond, JBondType::TripleBond};
            } else {// 双键
                sBondCandidates = {JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::SingleBond, JBondType::WaveBond,
                                   JBondType::DoubleBond};
            }
            // 检查键端原子是不是已经有双键
            bool allowReplace = true;
            auto check_allow_replace = [&](const size_t &_bid2) -> bool {
                if (_bid == _bid2) {
                    return true;
                }
                auto bond2 = mol->getBondById(_bid2);
                if (from == bond2->getAtomFrom() || from == bond2->getAtomTo()) {
                    if (JBondType::DoubleBond == bond2->getBondType()) {
                        allowReplace = false;
                        return false;
                    }
                }
                if (to == bond2->getAtomFrom() || to == bond2->getAtomTo()) {
                    if (JBondType::DoubleBond == bond2->getBondType()) {
                        allowReplace = false;
                        return false;
                    }
                }
                return true;
            };
            mol->safeTraverseBondsBreakIf(check_allow_replace);
            if (allowReplace) {
                const frac oldValence = bond->asValence();
                bond->setType(randSelect(sBondCandidates));
                atomValenceMap[from] += (bond->asValence() - oldValence);
                atomValenceMap[to] += (bond->asValence() - oldValence);
            }
        }
    };
    if (_replaceBond) {
        mol->safeTraverseBonds(replace_line_bond_type);
    }
    // 换原子类型
    auto replace_atom_type = [&](const size_t &_aid) -> void {
        static std::vector<ElementType> sAtomCandidates = {
                ElementType::H, ElementType::C,
                ElementType::N, ElementType::P, ElementType::B,
                ElementType::O, ElementType::S,
                ElementType::F, ElementType::Cl, ElementType::Br, ElementType::I
        };
        const auto &ele = randSelect(sAtomCandidates);
        if (atomValenceMap[_aid] <= ElementValenceData[ele]) {
            mol->getAtomById(_aid)->setElementType(ele);
        }
    };
    if (_replaceAtom) { mol->safeTraverseAtoms(replace_atom_type); }
    // 添加官能团
    auto add_group = [&](const size_t &_aid) {// COOH CHO etc
        auto atom = mol->getAtomById(_aid);
        if (atomValenceMap[_aid] == 1 && ElementType::C == atom->getElementType()) {
            addGroup(_aid);
        }
    };
    mol->safeTraverseAtoms(add_group);
    // 添加杂环
    std::unordered_set<size_t> usedAids;
    auto replace_bond_with_ring = [&](const size_t &_bid) {
        auto bond = mol->getBondById(_bid);
        if (byProb(0.2) && JBondType::SingleBond == bond->getBondType()) {
            size_t from = bond->getAtomFrom(), to = bond->getAtomTo();
            // 避免一个原子挂上好几个环导致排版引擎崩溃
            if (notExist(usedAids, from) && notExist(usedAids, from)) {
                if (_addAromaticRing && byProb(0.5)) {
                    addAromaticRing(_bid);
                } else if (_addCommonRing) {
                    addCommonRing(_bid);
                }
            }
            usedAids.insert(from);
            usedAids.insert(to);
        }
    };
    if (_addAromaticRing || _addCommonRing) {
        mol->safeTraverseBonds(replace_bond_with_ring);
    }
    // 打补丁：删掉 P#C B#C
    auto clear_invalid_bond = [&](const size_t &_bid) {
        auto bond = mol->getBondById(_bid);
        if (JBondType::TripleBond == bond->getBondType()) {
            auto fromAtom = mol->getAtomById(bond->getAtomFrom());
            auto ele = fromAtom->getElementType();
            if (ElementType::C != ele && ElementType::N != ele) {
                fromAtom->setElementType(ElementType::C);
            }
            auto toAtom = mol->getAtomById(bond->getAtomTo());
            ele = toAtom->getElementType();
            if (ElementType::C != ele && ElementType::N != ele) {
                toAtom->setElementType(ElementType::C);
            }
        }
    };
    mol->safeTraverseBonds(clear_invalid_bond);
    // 添加氢原子
    auto add_hydrogen = [&](const size_t &_aid) {
        auto atom = mol->getAtomById(_aid);
        if (ElementType::C != atom->getElementType()) {// 杂原子较大可能加氢
            if (byProb(_addHydrogenProb + 0.5)) {
                addHs(_aid);
            }
        } else if (byProb(_addHydrogenProb)) {// 碳原子按概率加氢
            addHs(_aid);
        }
    };
    mol->safeTraverseAtoms(add_hydrogen);
    // 楔形键
    updateNeighborInfoMap();
    std::unordered_map<size_t, bool> allowWedge;
    auto init_wedge_map = [&](const size_t &_aid) {
        allowWedge[_aid] = true;
    };
    mol->safeTraverseAtoms(init_wedge_map);
    auto add_wedge_bond = [&](const size_t &_aid) {
        auto atom = mol->getAtomById(_aid);
        bool allowThisWedge = true;
        if (neighborAtomsMap[_aid].size() == 4 && ElementType::C == atom->getElementType()) {
            for (auto &nid:neighborAtomsMap[_aid]) {
                if (!allowWedge[nid]) {
                    allowThisWedge = false;
                    break;
                }
            }
            if (allowThisWedge) {
                allowWedge[_aid] = false;
                std::vector<std::shared_ptr<JBond>> bonds;
                for (auto &bid:neighborBondsMap[_aid]) {
                    bonds.push_back(mol->getBondById(bid));
                }
                // 优先为 C-H 键加构型
                int okNeighbor = 0;
                for (auto &bond:bonds) {
                    if (okNeighbor == 2) {
                        break;
                    }
                    if (ElementType::H == mol->getAtomById(bond->getAtomFrom())->getElementType() ||
                        ElementType::H == mol->getAtomById(bond->getAtomTo())->getElementType()) {
                        if (bond->setFrom(_aid)) {
                            if (okNeighbor == 0)
                                bond->setType(JBondType::SolidWedgeBond);
                            else
                                bond->setType(JBondType::DashWedgeBond);
                            okNeighbor += 1;
                        }

                    }
                }
                while (okNeighbor < 2) {
                    auto &bond = randSelect(bonds);
                    if (bond->setFrom(_aid)) {
                        if (okNeighbor == 0)
                            bond->setType(JBondType::SolidWedgeBond);
                        else
                            bond->setType(JBondType::DashWedgeBond);
                        okNeighbor += 1;
                    }
                }
            }
        }
    };
    mol->safeTraverseAtoms(add_wedge_bond);
    // TODO: 波浪线
    // TODO: 添加不展开的字符串
}

void MolOp::addHs(const size_t &_aid) {
    if (atomValenceMap.empty()) {
        updateAtomValenceMap();
    }
    auto &valence = atomValenceMap.at(_aid);
    auto atom = mol->getAtomById(_aid);
    frac numOfH = ElementValenceData[atom->getElementType()] - valence;
//    std::cout<<atom->getAtomicNumber()<<","<<valence<<",";
//    std::cout<<numOfH<<std::endl;
    if (numOfH >= 1) {
        int hsNum = std::lround(numOfH.floatValue());
        for (int i = 0; i < hsNum; i++) {
            auto h = mol->addAtom(1);
            const size_t &hid = h->getId();
            mol->addBond(hid, _aid, JBondType::SingleBond);
            atomValenceMap[hid] = 1;
        }
        valence += numOfH;
    }
}

void MolOp::addGroup(const size_t &_aid) {
    std::vector<std::shared_ptr<JAtom>> newAtoms;
    std::vector<std::shared_ptr<JBond>> newBonds;
    int index = randInt() % 1;
    switch (index) {
        case 0: {// 羰基、酰基、醛基、羧基、巯基
            static std::vector<size_t> carbonyl = {8, 8, 8, 8, 16};
            static std::vector<size_t> acyl = {1, 8, 9, 16, 17, 53, 35};
            newAtoms = {
                    mol->addAtom(randSelect(carbonyl)),
                    mol->addAtom(randSelect(acyl)),
                    mol->addAtom(6)
            };
            newBonds = {
                    mol->addBond(_aid, newAtoms[2]->getId(),
                                 JBondType::SingleBond),// 连羧基碳
                    mol->addBond(newAtoms[2]->getId(), newAtoms[1]->getId(),
                                 JBondType::SingleBond),// 连羧基氢
                    mol->addBond(newAtoms[2]->getId(), newAtoms[0]->getId(),
                                 JBondType::DoubleBond)// 连羧基氧
            };
            break;
        }
        case 1:
        case 2: {
            break;
        }
        case 3: {
            break;
        }
        default:
            break;
    }
    updateAtomValenceMap(newBonds);
}

void MolOp::addAromaticRing(const size_t &_bid) {
    std::vector<std::shared_ptr<JAtom>> newAtoms;
    std::vector<std::shared_ptr<JBond>> newBonds;
    auto bond = mol->getBondById(_bid);
    auto from = bond->getAtomFrom(), to = bond->getAtomTo();
    removeBond(_bid);
    int index = randInt() % 9;
    switch (index) {
        case 0:
        case 1:
        case 2:
        case 3: {// 呋喃系
            std::vector<size_t> atomicNumbers = {7, 8, 15, 16};
            newAtoms = {
                    mol->addAtom(atomicNumbers[index]),
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(6), mol->addAtom(6)
            };
            std::unordered_set<size_t> atomInRing, bondInRing;
            for (auto &atom:newAtoms) {
                atomInRing.insert(atom->getId());
            }
            aromaticRingAids.push_back({std::move(atomInRing)});
            newBonds = {
                    mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId(),
                                 JBondType::SingleBond),
                    mol->addBond(newAtoms[0]->getId(), newAtoms[4]->getId(),
                                 JBondType::SingleBond),
                    mol->addBond(newAtoms[1]->getId(), newAtoms[2]->getId(),
                                 JBondType::DoubleBond),
                    mol->addBond(newAtoms[2]->getId(), newAtoms[3]->getId(),
                                 JBondType::SingleBond),
                    mol->addBond(newAtoms[3]->getId(), newAtoms[4]->getId(),
                                 JBondType::DoubleBond)
            };
            for (auto &bond:newBonds) {
                bondInRing.insert(bond->getId());
            }
            aromaticRingBids.push_back({std::move(bondInRing)});
            newBonds.push_back(mol->addBond(from, newAtoms[1 + randInt() % 2]->getId(),
                                            JBondType::SingleBond));
            newBonds.push_back(mol->addBond(to, newAtoms[3 + randInt() % 2]->getId(),
                                            JBondType::SingleBond));
            break;
        }
        case 4:
        case 5: {// 苯环系
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            newAtoms = {
                    mol->addAtom(6),
                    mol->addAtom(6),
                    mol->addAtom(randSelect(atomicNumbers)),
                    mol->addAtom(randSelect(atomicNumbers)),
                    mol->addAtom(randSelect(atomicNumbers)),
                    mol->addAtom(randSelect(atomicNumbers))
            };
            std::unordered_set<size_t> atomInRing, bondInRing;
            for (auto &atom:newAtoms) {
                atomInRing.insert(atom->getId());
            }
            aromaticRingAids.push_back({std::move(atomInRing)});
            size_t pos1 = newAtoms[0]->getId(), pos2 = newAtoms[1]->getId();
            std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
            newBonds = {
                    mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId(),
                                 JBondType::SingleBond),
                    mol->addBond(newAtoms[0]->getId(), newAtoms[5]->getId(),
                                 JBondType::DoubleBond),
                    mol->addBond(newAtoms[1]->getId(), newAtoms[2]->getId(),
                                 JBondType::DoubleBond),
                    mol->addBond(newAtoms[2]->getId(), newAtoms[3]->getId(),
                                 JBondType::SingleBond),
                    mol->addBond(newAtoms[3]->getId(), newAtoms[4]->getId(),
                                 JBondType::DoubleBond),
                    mol->addBond(newAtoms[4]->getId(), newAtoms[5]->getId(),
                                 JBondType::SingleBond)
            };
            for (auto &bond:newBonds) {
                bondInRing.insert(bond->getId());
            }
            aromaticRingBids.push_back({std::move(bondInRing)});
            newBonds.push_back(mol->addBond(from, pos1, JBondType::SingleBond));
            newBonds.push_back(mol->addBond(to, pos2, JBondType::SingleBond));
            break;
        }
        case 6: {//萘
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            std::vector<size_t> poses;
            const size_t reactSiteNum = 8;
            for (int i = 0; i < reactSiteNum; i++) {
                auto atom = mol->addAtom(randSelect(atomicNumbers));
                newAtoms.push_back(atom);
                if (ElementType::C == atom->getElementType()) {
                    poses.push_back(atom->getId());
                }
            }
            if (poses.size() < 2) {
                size_t pos1 = randInt() % (reactSiteNum / 2);
                size_t pos2 = reactSiteNum / 2 + randInt() % (reactSiteNum / 2);
                newAtoms[pos1]->setElementType(ElementType::C);
                newAtoms[pos2]->setElementType(ElementType::C);
                poses = {newAtoms[pos1]->getId(), newAtoms[pos2]->getId()};
            } else {
                std::shuffle(poses.begin(), poses.end(), std::default_random_engine());
            }
            // 非反应位点
            for (int i = 0; i < 2; i++) {
                newAtoms.push_back(mol->addAtom(6));
            }
            std::vector<std::pair<size_t, size_t>> sBond = {
                    {2, 3},
                    {4, 10},
                    {5, 6},
                    {7, 8},
                    {9, 10},
                    {1, 9}
            };
            std::vector<std::pair<size_t, size_t>> dBond = {
                    {1, 2},
                    {3, 4},
                    {5, 10},
                    {6, 7},
                    {8, 9}
            };
            for (auto&[a1, a2]:sBond) {
                newBonds.push_back(mol->addBond(newAtoms[a1 - 1]->getId(),
                                                newAtoms[a2 - 1]->getId()));
            }
            for (auto&[a1, a2]:dBond) {
                newBonds.push_back(mol->addBond(newAtoms[a1 - 1]->getId(),
                                                newAtoms[a2 - 1]->getId(),
                                                JBondType::DoubleBond));
            }
            // TODO: 把环信息写入 aromaticRingAids 和 aromaticRingBids
            newBonds.push_back(mol->addBond(from, poses[0]));
            newBonds.push_back(mol->addBond(to, poses[1]));
            break;
        }
        case 7: {//蒽
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            std::vector<size_t> poses;
            const size_t reactSiteNum = 10;
            for (int i = 0; i < reactSiteNum; i++) {
                auto atom = mol->addAtom(randSelect(atomicNumbers));
                newAtoms.push_back(atom);
                if (ElementType::C == atom->getElementType()) {
                    poses.push_back(atom->getId());
                }
            }
            if (poses.size() < 2) {
                size_t pos1 = randInt() % (reactSiteNum / 2);
                size_t pos2 = reactSiteNum / 2 + randInt() % (reactSiteNum / 2);
                newAtoms[pos1]->setElementType(ElementType::C);
                newAtoms[pos2]->setElementType(ElementType::C);
                poses = {newAtoms[pos1]->getId(), newAtoms[pos2]->getId()};
            } else {
                std::shuffle(poses.begin(), poses.end(), std::default_random_engine());
            }
            // 非反应位点
            for (int i = 0; i < 4; i++) {
                newAtoms.push_back(mol->addAtom(6));
            }
            std::vector<std::pair<size_t, size_t>> sBond = {
                    {11, 1},
                    {2,  3},
                    {4,  12},
                    {10, 13},
                    {5,  6},
                    {7,  8},
                    {14, 9},
                    {11, 12},
                    {13, 14}
            };
            std::vector<std::pair<size_t, size_t>> dBond = {
                    {1,  2},
                    {3,  4},
                    {12, 10},
                    {13, 5},
                    {6,  7},
                    {8,  14},
                    {9,  11}
            };
            for (auto&[a1, a2]:sBond) {
                newBonds.push_back(mol->addBond(newAtoms[a1 - 1]->getId(),
                                                newAtoms[a2 - 1]->getId()));
            }
            for (auto&[a1, a2]:dBond) {
                newBonds.push_back(mol->addBond(newAtoms[a1 - 1]->getId(),
                                                newAtoms[a2 - 1]->getId(),
                                                JBondType::DoubleBond));
            }
            newBonds.push_back(mol->addBond(from, poses[0]));
            newBonds.push_back(mol->addBond(to, poses[1]));
            break;
        }
        case 8: {//菲
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            std::vector<size_t> poses;
            const size_t reactSiteNum = 10;
            for (int i = 0; i < reactSiteNum; i++) {
                auto atom = mol->addAtom(randSelect(atomicNumbers));
                newAtoms.push_back(atom);
                if (ElementType::C == atom->getElementType()) {
                    poses.push_back(atom->getId());
                }
            }
            if (poses.size() < 2) {
                size_t pos1 = randInt() % (reactSiteNum / 2);
                size_t pos2 = reactSiteNum / 2 + randInt() % (reactSiteNum / 2);
                newAtoms[pos1]->setElementType(ElementType::C);
                newAtoms[pos2]->setElementType(ElementType::C);
                poses = {newAtoms[pos1]->getId(), newAtoms[pos2]->getId()};
            } else {
                std::shuffle(poses.begin(), poses.end(), std::default_random_engine());
            }
            // 非反应位点
            for (int i = 0; i < 4; i++) {
                newAtoms.push_back(mol->addAtom(6));
            }
            std::vector<std::pair<size_t, size_t>> sBond = {
                    {11, 1},
                    {2,  3},
                    {4,  12},
                    {12, 13},
                    {13, 5},
                    {6,  7},
                    {8,  14},
                    {14, 9},
                    {10, 11}
            };
            std::vector<std::pair<size_t, size_t>> dBond = {
                    {1,  2},
                    {3,  4},
                    {5,  6},
                    {7,  8},
                    {13, 14},
                    {9,  10},
                    {11, 12}
            };
            for (auto&[a1, a2]:sBond) {
                newBonds.push_back(mol->addBond(newAtoms[a1 - 1]->getId(),
                                                newAtoms[a2 - 1]->getId()));
            }
            for (auto&[a1, a2]:dBond) {
                newBonds.push_back(mol->addBond(newAtoms[a1 - 1]->getId(),
                                                newAtoms[a2 - 1]->getId(),
                                                JBondType::DoubleBond));
            }
            newBonds.push_back(mol->addBond(from, poses[0]));
            newBonds.push_back(mol->addBond(to, poses[1]));
            break;
        }
        case 9: {
            break;
        }
        case 10: {
            break;
        }
        case 11: {
            break;
        }
        case 12: {
            break;
        }
        default:
            break;
    }
    updateAtomValenceMap(newBonds);
}

void MolOp::addCommonRing(const size_t &_bid) {
    std::vector<std::shared_ptr<JAtom>> newAtoms;
    std::vector<std::shared_ptr<JBond>> newBonds;
    auto bond = mol->getBondById(_bid);
    auto from = bond->getAtomFrom(), to = bond->getAtomTo();
    removeBond(_bid);
    std::vector<size_t> nodeNums = {3, 3, 4, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7};
    auto &nodeNum = randSelect(nodeNums);
    std::vector<size_t> atomicNumbers = {5, 6, 7, 8, 15, 16};
    switch (nodeNum) {
        case 3: {// 三元环
            newAtoms = {
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(randSelect(atomicNumbers))
            };
            std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
            newBonds = {
                    mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    mol->addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    mol->addBond(newAtoms[2]->getId(), newAtoms[0]->getId()),
                    mol->addBond(from, newAtoms[randInt() % 2]->getId()),
                    mol->addBond(to, newAtoms[randInt() % 2]->getId())
            };
            break;
        }
        case 4: {// 四元环
            newAtoms = {
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(6), mol->addAtom(randSelect(atomicNumbers))
            };
            std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
            newBonds = {
                    mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    mol->addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    mol->addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    mol->addBond(newAtoms[3]->getId(), newAtoms[0]->getId()),
                    mol->addBond(from, newAtoms[randInt() % 3]->getId()),
                    mol->addBond(to, newAtoms[randInt() % 3]->getId())
            };
            break;
        }
        case 5: {// 五元环
            newAtoms = {
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(randSelect(atomicNumbers))
            };
            std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
            newBonds = {
                    mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    mol->addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    mol->addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    mol->addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    mol->addBond(newAtoms[4]->getId(), newAtoms[0]->getId()),
                    mol->addBond(from, newAtoms[randInt() % 4]->getId()),
                    mol->addBond(to, newAtoms[randInt() % 4]->getId())
            };
            break;
        }
        case 6: {// 六元环
            newAtoms = {
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(6), mol->addAtom(randSelect(atomicNumbers))
            };
            std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
//            static std::vector<std::vector<std::pair<float, float>>> c6Coords2d = {
//                    {{78,  324}, {132, 224}, {265, 268}, {352, 237}, {292, 339}, {172, 291}},
//                    {{433, 231}, {522, 260}, {643, 221}, {699, 318}, {615, 286}, {483, 328}},
//                    {{475, 317}, {522, 371}, {583, 371}, {629, 321}, {608, 415}, {500, 413}}
//            };
//            for (size_t i = 0; i < 6; i++) {
//                const auto &coord_template = randSelect(c6Coords2d);
//                const auto&[x, y]=coord_template[i];
//                newAtoms[i]->setCoord2d(x, y);
//            }
            newBonds = {
                    mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    mol->addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    mol->addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    mol->addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    mol->addBond(newAtoms[4]->getId(), newAtoms[5]->getId()),
                    mol->addBond(newAtoms[5]->getId(), newAtoms[0]->getId()),
                    mol->addBond(from, newAtoms[randInt() % 5]->getId()),
                    mol->addBond(to, newAtoms[randInt() % 5]->getId())
            };
            break;
        }
        case 7: {// 七元环
            newAtoms = {
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(randSelect(atomicNumbers))
            };
            std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
            newBonds = {
                    mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    mol->addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    mol->addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    mol->addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    mol->addBond(newAtoms[4]->getId(), newAtoms[5]->getId()),
                    mol->addBond(newAtoms[5]->getId(), newAtoms[6]->getId()),
                    mol->addBond(newAtoms[6]->getId(), newAtoms[0]->getId()),
                    mol->addBond(from, newAtoms[randInt() % 6]->getId()),
                    mol->addBond(to, newAtoms[randInt() % 6]->getId())
            };
            break;
        }
        case 100: {
            // FIXME: coodgen2d 的桥环渲染有问题
            newAtoms = {
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(6), mol->addAtom(6),
                    mol->addAtom(randSelect(atomicNumbers))
            };
            std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
            newBonds = {
                    mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    mol->addBond(newAtoms[1]->getId(), newAtoms[2]->getId(),
                                 JBondType::DoubleBond),
                    mol->addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    mol->addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    mol->addBond(newAtoms[4]->getId(), newAtoms[5]->getId(),
                                 JBondType::DoubleBond),
                    mol->addBond(newAtoms[5]->getId(), newAtoms[0]->getId()),
                    mol->addBond(from, newAtoms[0 + randInt() % 3]->getId()),
                    mol->addBond(to, newAtoms[3 + randInt() % 3]->getId()),
                    mol->addBond(newAtoms[0]->getId(), newAtoms[6]->getId()),
                    mol->addBond(newAtoms[3]->getId(), newAtoms[6]->getId())
            };
            break;
        }
        default:
            break;
    }
    updateAtomValenceMap(newBonds);
}

void MolOp::updateNeighborInfoMap() {
    neighborAtomsMap.clear();
    neighborBondsMap.clear();
    auto get_neighbor = [&](const size_t &_bid) {
        auto bond = mol->getBondById(_bid);
        auto from = bond->getAtomFrom(), to = bond->getAtomTo();
        neighborAtomsMap[from].insert(to);
        neighborAtomsMap[to].insert(from);
        neighborBondsMap[from].insert(_bid);
        neighborBondsMap[to].insert(_bid);
    };
    mol->safeTraverseBonds(get_neighbor);
}


const std::vector<std::vector<std::unordered_set<size_t>>> &
MolOp::getAromaticRings(bool _retAid) const {
    return _retAid ? aromaticRingAids : aromaticRingBids;
}

void MolOp::updateAtomValenceMap() {
    atomValenceMap.clear();
    auto add_valence4atom = [&](const size_t &_aid, const frac &_valence) {
        if (notExist(atomValenceMap, _aid)) {
            atomValenceMap[_aid] = _valence;
        } else {
            atomValenceMap[_aid] += _valence;
        }
    };
    auto add_valence_from_bond = [&](const size_t &_bid) {
        auto bond = mol->getBondById(_bid);
//        std::cout<<bond->asValence()<<std::endl;
        add_valence4atom(bond->getAtomFrom(), bond->asValence());
        add_valence4atom(bond->getAtomTo(), bond->asValence());
    };
    mol->safeTraverseBonds(add_valence_from_bond);
    // 处理没有化学键的情况，比如加氢之前的甲烷
    auto handle_atom_alone = [&](const size_t &_aid) {
        if (notExist(atomValenceMap, _aid)) {
            atomValenceMap[_aid] = 0;
        }
    };
    mol->safeTraverseAtoms(handle_atom_alone);
}


void MolOp::addValence4Atom(const size_t &_aid, const frac &_valence) {
    if (notExist(atomValenceMap, _aid)) {
        atomValenceMap[_aid] = _valence;
    } else {
        atomValenceMap[_aid] += _valence;
    }
}


void MolOp::removeBond(const size_t &_bid) {
    auto bond = mol->getBondById(_bid);
    if (bond) {
        auto oldVal = bond->asValence();
        atomValenceMap[bond->getAtomFrom()] -= oldVal;
        atomValenceMap[bond->getAtomTo()] -= oldVal;
        mol->removeBond(_bid);
    }
}

void MolOp::updateAtomValenceMap(const std::vector<std::shared_ptr<JBond>> &_bonds) {
    for (auto &bond:_bonds) {
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
    }
}

MolOp::MolOp(std::shared_ptr<JMol> _mol) : MolHolder(std::move(_mol)) {

}

bool MolOp::markAsDouble(const size_t &_bid) {
    if (atomValenceMap.empty()) {
        updateAtomValenceMap();
    }
    if (dbCounter.empty()) {
        updateDoubleBondCounter();
    }
    auto bond = mol->getBondById(_bid);
    auto a1 = mol->getAtomById(bond->getAtomFrom());
    auto a2 = mol->getAtomById(bond->getAtomTo());
    auto &va1 = atomValenceMap[a1->getId()];
    auto &va2 = atomValenceMap[a2->getId()];
    const auto &oa1 = ElementValenceData[a1->getElementType()];
    const auto &oa2 = ElementValenceData[a2->getElementType()];
    auto d1 = oa1 - va1, d2 = oa2 - va2;
    if (d1 >= 1 && d2 >= 1 &&
        0 == dbCounter[a1->getId()] && 0 == dbCounter[a2->getId()]) {
        dbCounter[a1->getId()] += 1;
        dbCounter[a2->getId()] += 1;
        va1 += (frac(2) - bond->asValence());
        va2 += (frac(2) - bond->asValence());
        bond->setType(JBondType::DoubleBond);
        return true;
    }
    return false;
}

void MolOp::updateDoubleBondCounter() {
    dbCounter.clear();
    auto add_db_num = [&](const size_t &_bid) -> void {
        auto bond = mol->getBondById(_bid);
        bool isDb = JBondType::DoubleBond == bond->getBondType();
        auto a1 = bond->getAtomFrom(), a2 = bond->getAtomTo();
        if (notExist(dbCounter, a1)) {
            dbCounter[a1] = 0;
        }
        if (notExist(dbCounter, a2)) {
            dbCounter[a2] = 0;
        }
        if (isDb) {
            dbCounter[a1] += 1;
            dbCounter[a2] += 1;
        }
    };
    mol->safeTraverseBonds(add_db_num);
}

frac MolOp::getValByAtomId(const size_t &_aid) const {
    auto it = atomValenceMap.find(_aid);
    if (it == atomValenceMap.end())
        return 0;
    else
        return it->second;
}
