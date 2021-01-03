#include "mol_op.hpp"
std::pair<std::vector<std::shared_ptr<JAtom>>, std::vector<std::shared_ptr<JBond>>>
MolOp::makeCarbonRing(const size_t &_nodeNum,
                     const std::vector<std::pair<size_t, size_t>> &_singleLink,
                     const std::vector<std::pair<size_t, size_t>> &_doubleLink,
                     const std::vector<size_t> &_mixAtomicNumbers) {

    std::vector<std::shared_ptr<JAtom>> newAtoms;
    for (size_t i = 0; i < _nodeNum; i++) {
        newAtoms.push_back(addAtom(6));
    }
    std::vector<std::shared_ptr<JBond>> newBonds;
    for (auto&[a1, a2]:_singleLink) {
        newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                   newAtoms[a2 - 1]->getId()));
    }
    for (auto&[a1, a2]:_doubleLink) {
        newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                   newAtoms[a2 - 1]->getId(),
                                   JBondType::DoubleBond));
    }
    if (!_mixAtomicNumbers.empty()) {
        for (auto &atom:newAtoms) {
            // TODO: 1：选杂原子 2：检查是否可以插入
        }
    }
    return {std::move(newAtoms), std::move(newBonds)};
}

void MolOp::clear() {
    mol->clear();
    atomValenceMap.clear();
    neighborAtomsMap.clear();
    neighborBondsMap.clear();
    aromaticRingAids.clear();
    aromaticRingBids.clear();
}

void MolOp::randomize(const float &_addHydrogenProb, bool _replaceBond, bool _replaceAtom,
                     bool _addAromaticRing, bool _addCommonRing) {
    updateAtomValenceMap();
    // 换化学键类型
    auto replaceBondType = [&](const size_t &_bid) {
        const auto &bond = bondsMap[_bid];
        const size_t &from = bond->getAtomFrom(), to = bond->getAtomTo();
        const auto &fromAtom = atomsMap[from];
        const auto &toAtom = atomsMap[to];
        const frac deltaFrom = ElementValenceData[fromAtom->getElementType()]
                               - atomValenceMap[from];
        const frac deltaTo = ElementValenceData[toAtom->getElementType()]
                             - atomValenceMap[to];
        if (deltaFrom >= 1 && deltaTo >= 1) {
            frac delta = std::min(deltaFrom, deltaTo);
            std::vector<JBondType> sBondCandidates;
            if (delta >= 2) {// 三键、双键
                sBondCandidates = {JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::DoubleBond, JBondType::TripleBond};
            } else {// 双键
                sBondCandidates = {JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::SingleBond, JBondType::SingleBond,
                                   JBondType::DoubleBond};
            }
            // 检查键端原子是不是已经有双键
            bool allowReplace = true;
            for (auto&[id2, bond2]:bondsMap) {
                if (_bid == id2) {
                    continue;
                }
                if (from == bond2->getAtomFrom() || from == bond2->getAtomTo()) {
                    if (JBondType::DoubleBond == bond2->getBondType()) {
                        allowReplace = false;
                        break;
                    }
                }
                if (to == bond2->getAtomFrom() || to == bond2->getAtomTo()) {
                    if (JBondType::DoubleBond == bond2->getBondType()) {
                        allowReplace = false;
                        break;
                    }
                }
            }
            if (allowReplace) {
                const frac oldValence = bond->asValence();
                bond->setType(randSelect(sBondCandidates));
                atomValenceMap[from] += (bond->asValence() - oldValence);
                atomValenceMap[to] += (bond->asValence() - oldValence);
            }
        }
    };
    if (_replaceBond)
        safeTraverseBonds(replaceBondType);
    // 换原子类型
    if (_replaceAtom) {
        static std::vector<ElementType> sAtomCandidates = {
                ElementType::H, ElementType::C,
                ElementType::N, ElementType::P, ElementType::B,
                ElementType::O, ElementType::S,
                ElementType::F, ElementType::Cl, ElementType::Br, ElementType::I
        };
        for (auto&[id, atom]:atomsMap) {
            const auto &ele = randSelect(sAtomCandidates);
            if (atomValenceMap[id] <= ElementValenceData[ele]) {
                atom->setElementType(ele);
            }
        }
    }
    // 添加官能团
    auto add_group = [&](const size_t &_aid) {// COOH CHO etc
        const auto &atom = atomsMap[_aid];
        if (atomValenceMap[_aid] == 1 && ElementType::C == atom->getElementType()) {
            addGroup(_aid);
        }
    };
    safeTraverseAtoms(add_group);
    // 添加杂环
    std::unordered_set<size_t> usedAids;
    auto replaceBondWithRing = [&](const size_t &_bid) {
        const auto &bond = bondsMap[_bid];
        if (byProb(0.2) && JBondType::SingleBond == bond->getBondType()) {
            size_t from = bondsMap[_bid]->getAtomFrom(), to = bondsMap[_bid]->getAtomTo();
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
    if (_addAromaticRing || _addCommonRing)
        safeTraverseBonds(replaceBondWithRing);
    // 打补丁：删掉 P#C B#C
    auto clearStrangeBond = [&](const size_t &_bid) {
        const auto &bond = bondsMap[_bid];
        if (JBondType::TripleBond == bond->getBondType()) {
            auto ele = atomsMap[bond->getAtomFrom()]->getElementType();
            if (ElementType::C != ele && ElementType::N != ele) {
                atomsMap[bond->getAtomFrom()]->setElementType(ElementType::C);
            }
            ele = atomsMap[bond->getAtomTo()]->getElementType();
            if (ElementType::C != ele && ElementType::N != ele) {
                atomsMap[bond->getAtomTo()]->setElementType(ElementType::C);
            }
        }
    };
    safeTraverseBonds(clearStrangeBond);
    // 添加氢原子
    auto add_hydrogen = [&](const size_t &_aid) {
        const auto &atom = atomsMap[_aid];
        if (ElementType::C != atom->getElementType()) {// 杂原子较大可能加氢
            if (byProb(_addHydrogenProb + 0.5)) {
                addHs(_aid);
            }
        } else if (byProb(_addHydrogenProb)) {// 碳原子按概率加氢
            addHs(_aid);
        }
    };
    safeTraverseAtoms(add_hydrogen);
    // 楔形键
    updateNeighborInfoMap();
    std::unordered_map<size_t, bool> allowWedge;
    for (auto&[id, _]:atomsMap) {
        allowWedge[id] = true;
    }
    for (auto&[aid, atom]:atomsMap) {
        bool allowThisWedge = true;
        if (neighborAtomsMap[aid].size() == 4 && ElementType::C == atom->getElementType()) {
            for (auto &nid:neighborAtomsMap[aid]) {
                if (!allowWedge[nid]) {
                    allowThisWedge = false;
                    break;
                }
            }
            if (allowThisWedge) {
                allowWedge[aid] = false;
                std::vector<std::shared_ptr<JBond>> bonds;
                for (auto &bid:neighborBondsMap[aid]) {
                    bonds.push_back(bondsMap[bid]);
                }
                // 优先为 C-H 键加构型
                int okNeighbor = 0;
                for (auto &bond:bonds) {
                    if (okNeighbor == 2) {
                        break;
                    }
                    if (ElementType::H == atomsMap[bond->getAtomFrom()]->getElementType() ||
                        ElementType::H == atomsMap[bond->getAtomTo()]->getElementType()) {
                        if (bond->setFrom(aid)) {
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
                    if (bond->setFrom(aid)) {
                        if (okNeighbor == 0)
                            bond->setType(JBondType::SolidWedgeBond);
                        else
                            bond->setType(JBondType::DashWedgeBond);
                        okNeighbor += 1;
                    }
                }
            }
        }
    }
    // TODO: 波浪线
    // TODO: 添加不展开的字符串
}
void MolOp::addHs(const size_t &_aid) {
    auto &valence = atomValenceMap.at(_aid);
    const auto &atom = atomsMap[_aid];
    frac numOfH = ElementValenceData[atom->getElementType()] - valence;
    if (numOfH >= 1) {
        int hsNum = std::lround(numOfH.floatValue());
        for (int i = 0; i < hsNum; i++) {
            auto h = addAtom(1);
            const size_t &hid = h->getId();
            addBond(hid, _aid, JBondType::SingleBond);
            atomValenceMap[hid] = 1;
        }
        valence += numOfH;
    }
}
void MolOp::addGroup(const size_t &_aid) {
    std::vector<std::shared_ptr<JAtom>> newAtoms;
    std::vector<std::shared_ptr<JBond>> newBonds;
    int index = rand() % 1;
    switch (index) {
        case 0: {// 羰基、酰基、醛基、羧基、巯基
            static std::vector<size_t> carbonyl = {8, 8, 8, 8, 16};
            static std::vector<size_t> acyl = {1, 8, 9, 16, 17, 53, 35};
            newAtoms = {
                    addAtom(randSelect(carbonyl)),
                    addAtom(randSelect(acyl)),
                    addAtom(6)
            };
            newBonds = {
                    addBond(_aid, newAtoms[2]->getId(),
                            JBondType::SingleBond),// 连羧基碳
                    addBond(newAtoms[2]->getId(), newAtoms[1]->getId(),
                            JBondType::SingleBond),// 连羧基氢
                    addBond(newAtoms[2]->getId(), newAtoms[0]->getId(),
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
    auto from = bondsMap[_bid]->getAtomFrom(), to = bondsMap[_bid]->getAtomTo();
    removeBond(_bid);
    int index = rand() % 9;
    switch (index) {
        case 0:
        case 1:
        case 2:
        case 3: {// 呋喃系
            std::vector<size_t> atomicNumbers = {7, 8, 15, 16};
            newAtoms = {
                    addAtom(atomicNumbers[index]),
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6)
            };
            std::unordered_set<size_t> atomInRing, bondInRing;
            for (auto &atom:newAtoms) {
                atomInRing.insert(atom->getId());
            }
            aromaticRingAids.push_back({std::move(atomInRing)});
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[0]->getId(), newAtoms[4]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId(),
                            JBondType::DoubleBond)
            };
            for (auto &bond:newBonds) {
                bondInRing.insert(bond->getId());
            }
            aromaticRingBids.push_back({std::move(bondInRing)});
            newBonds.push_back(addBond(from, newAtoms[1 + rand() % 2]->getId(),
                                       JBondType::SingleBond));
            newBonds.push_back(addBond(to, newAtoms[3 + rand() % 2]->getId(),
                                       JBondType::SingleBond));
            break;
        }
        case 4:
        case 5: {// 苯环系
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            newAtoms = {
                    addAtom(6),
                    addAtom(6),
                    addAtom(randSelect(atomicNumbers)),
                    addAtom(randSelect(atomicNumbers)),
                    addAtom(randSelect(atomicNumbers)),
                    addAtom(randSelect(atomicNumbers))
            };
            std::unordered_set<size_t> atomInRing, bondInRing;
            for (auto &atom:newAtoms) {
                atomInRing.insert(atom->getId());
            }
            aromaticRingAids.push_back({std::move(atomInRing)});
            size_t pos1 = newAtoms[0]->getId(), pos2 = newAtoms[1]->getId();
            std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[0]->getId(), newAtoms[5]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId(),
                            JBondType::SingleBond),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[4]->getId(), newAtoms[5]->getId(),
                            JBondType::SingleBond)
            };
            for (auto &bond:newBonds) {
                bondInRing.insert(bond->getId());
            }
            aromaticRingBids.push_back({std::move(bondInRing)});
            newBonds.push_back(addBond(from, pos1, JBondType::SingleBond));
            newBonds.push_back(addBond(to, pos2, JBondType::SingleBond));
            break;
        }
        case 6: {//萘
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            std::vector<size_t> poses;
            const size_t reactSiteNum = 8;
            for (int i = 0; i < reactSiteNum; i++) {
                auto atom = addAtom(randSelect(atomicNumbers));
                newAtoms.push_back(atom);
                if (ElementType::C == atom->getElementType()) {
                    poses.push_back(atom->getId());
                }
            }
            if (poses.size() < 2) {
                size_t pos1 = rand() % (reactSiteNum / 2);
                size_t pos2 = reactSiteNum / 2 + rand() % (reactSiteNum / 2);
                newAtoms[pos1]->setElementType(ElementType::C);
                newAtoms[pos2]->setElementType(ElementType::C);
                poses = {newAtoms[pos1]->getId(), newAtoms[pos2]->getId()};
            } else {
                std::shuffle(poses.begin(), poses.end(), std::default_random_engine());
            }
            // 非反应位点
            for (int i = 0; i < 2; i++) {
                newAtoms.push_back(addAtom(6));
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
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId()));
            }
            for (auto&[a1, a2]:dBond) {
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId(),
                                           JBondType::DoubleBond));
            }
            // TODO: 把环信息写入 aromaticRingAids 和 aromaticRingBids
            newBonds.push_back(addBond(from, poses[0]));
            newBonds.push_back(addBond(to, poses[1]));
            break;
        }
        case 7: {//蒽
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            std::vector<size_t> poses;
            const size_t reactSiteNum = 10;
            for (int i = 0; i < reactSiteNum; i++) {
                auto atom = addAtom(randSelect(atomicNumbers));
                newAtoms.push_back(atom);
                if (ElementType::C == atom->getElementType()) {
                    poses.push_back(atom->getId());
                }
            }
            if (poses.size() < 2) {
                size_t pos1 = rand() % (reactSiteNum / 2);
                size_t pos2 = reactSiteNum / 2 + rand() % (reactSiteNum / 2);
                newAtoms[pos1]->setElementType(ElementType::C);
                newAtoms[pos2]->setElementType(ElementType::C);
                poses = {newAtoms[pos1]->getId(), newAtoms[pos2]->getId()};
            } else {
                std::shuffle(poses.begin(), poses.end(), std::default_random_engine());
            }
            // 非反应位点
            for (int i = 0; i < 4; i++) {
                newAtoms.push_back(addAtom(6));
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
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId()));
            }
            for (auto&[a1, a2]:dBond) {
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId(),
                                           JBondType::DoubleBond));
            }
            newBonds.push_back(addBond(from, poses[0]));
            newBonds.push_back(addBond(to, poses[1]));
            break;
        }
        case 8: {//菲
            std::vector<size_t> atomicNumbers = {6, 6, 6, 7};
            std::vector<size_t> poses;
            const size_t reactSiteNum = 10;
            for (int i = 0; i < reactSiteNum; i++) {
                auto atom = addAtom(randSelect(atomicNumbers));
                newAtoms.push_back(atom);
                if (ElementType::C == atom->getElementType()) {
                    poses.push_back(atom->getId());
                }
            }
            if (poses.size() < 2) {
                size_t pos1 = rand() % (reactSiteNum / 2);
                size_t pos2 = reactSiteNum / 2 + rand() % (reactSiteNum / 2);
                newAtoms[pos1]->setElementType(ElementType::C);
                newAtoms[pos2]->setElementType(ElementType::C);
                poses = {newAtoms[pos1]->getId(), newAtoms[pos2]->getId()};
            } else {
                std::shuffle(poses.begin(), poses.end(), std::default_random_engine());
            }
            // 非反应位点
            for (int i = 0; i < 4; i++) {
                newAtoms.push_back(addAtom(6));
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
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId()));
            }
            for (auto&[a1, a2]:dBond) {
                newBonds.push_back(addBond(newAtoms[a1 - 1]->getId(),
                                           newAtoms[a2 - 1]->getId(),
                                           JBondType::DoubleBond));
            }
            newBonds.push_back(addBond(from, poses[0]));
            newBonds.push_back(addBond(to, poses[1]));
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
    auto from = bondsMap[_bid]->getAtomFrom(), to = bondsMap[_bid]->getAtomTo();
    removeBond(_bid);
    std::vector<size_t> nodeNums = {3, 3, 4, 5, 5, 5, 6, 6, 6, 6, 6, 7, 7};
    size_t &nodeNum = randSelect(nodeNums);
    std::vector<size_t> atomicNumbers = {5, 6, 7, 8, 15, 16};
    switch (nodeNum) {
        case 3: {// 三元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 2]->getId()),
                    addBond(to, newAtoms[rand() % 2]->getId())
            };
            break;
        }
        case 4: {// 四元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 3]->getId()),
                    addBond(to, newAtoms[rand() % 3]->getId())
            };
            break;
        }
        case 5: {// 五元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    addBond(newAtoms[4]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 4]->getId()),
                    addBond(to, newAtoms[rand() % 4]->getId())
            };
            break;
        }
        case 6: {// 六元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    addBond(newAtoms[4]->getId(), newAtoms[5]->getId()),
                    addBond(newAtoms[5]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 5]->getId()),
                    addBond(to, newAtoms[rand() % 5]->getId())
            };
            break;
        }
        case 7: {// 七元环
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    addBond(newAtoms[4]->getId(), newAtoms[5]->getId()),
                    addBond(newAtoms[5]->getId(), newAtoms[6]->getId()),
                    addBond(newAtoms[6]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[rand() % 6]->getId()),
                    addBond(to, newAtoms[rand() % 6]->getId())
            };
            break;
        }
        case 100: {
            // FIXME: coodgen2d 的桥环渲染有问题
            newAtoms = {
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(6), addAtom(6),
                    addAtom(randSelect(atomicNumbers))
            };
            newBonds = {
                    addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                    addBond(newAtoms[1]->getId(), newAtoms[2]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                    addBond(newAtoms[4]->getId(), newAtoms[5]->getId(),
                            JBondType::DoubleBond),
                    addBond(newAtoms[5]->getId(), newAtoms[0]->getId()),
                    addBond(from, newAtoms[0 + rand() % 3]->getId()),
                    addBond(to, newAtoms[3 + rand() % 3]->getId()),
                    addBond(newAtoms[0]->getId(), newAtoms[6]->getId()),
                    addBond(newAtoms[3]->getId(), newAtoms[6]->getId())
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
    for (auto&[bid, bond]:bondsMap) {
        auto from = bond->getAtomFrom(), to = bond->getAtomTo();
        neighborAtomsMap[from].insert(to);
        neighborAtomsMap[to].insert(from);
        neighborBondsMap[from].insert(bid);
        neighborBondsMap[to].insert(bid);
    }
}


const std::vector<std::vector<std::unordered_set<size_t>>> &
MolOp::getAromaticRings(bool _retAid) const {
    return _retAid ? aromaticRingAids : aromaticRingBids;
}

void MolOp::updateAtomValenceMap() {
    atomValenceMap.clear();
    auto addValence4Atom = [&](const size_t &_aid, const frac &_valence) {
        if (notExist(atomValenceMap, _aid)) {
            atomValenceMap[_aid] = _valence;
        } else {
            atomValenceMap[_aid] += _valence;
        }
    };
    for (auto&[id, bond]:bondsMap) {
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
    }
    // 处理没有化学键的情况，比如加氢之前的甲烷
    for (auto&[id, atom]:atomsMap) {
        if (notExist(atomValenceMap, id)) {
            atomValenceMap[id] = 0;
        }
    }
}

void MolOp::updateAtomValenceMap(const std::vector<size_t> &_bids) {
    for (auto &bid:_bids) {
        const auto &bond = bondsMap[bid];
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
    }
}

void MolOp::addValence4Atom(const size_t &_aid, const frac &_valence) {
    if (notExist(atomValenceMap, _aid)) {
        atomValenceMap[_aid] = _valence;
    } else {
        atomValenceMap[_aid] += _valence;
    }
}


void MolOp::removeBond(const size_t &_bid) {
    auto it = bondsMap.find(_bid);
    if (bondsMap.end() != it) {
        auto oldVal = it->second->asValence();
        atomValenceMap[it->second->getAtomFrom()] -= oldVal;
        atomValenceMap[it->second->getAtomTo()] -= oldVal;
        bondsMap.erase(it);
    }
}

void MolOp::updateAtomValenceMap(const std::vector<std::shared_ptr<JBond>> &_bonds) {
    for (auto &bond:_bonds) {
        addValence4Atom(bond->getAtomFrom(), bond->asValence());
        addValence4Atom(bond->getAtomTo(), bond->asValence());
    }
}
