#include "graph_composer.hpp"
#include "../chem/jmol_adapter.hpp"

#include <QDebug>

#include <map>
#include <stdexcept>

struct pair_hash {
    template<class T1, class T2>
    inline std::size_t operator()(const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

std::shared_ptr<xgd::JMol> xgd::GraphComposer::compose(const std::vector<OCRItem> &_items) {
//    xgd::ComposerHelper helper;
//    return helper.compose(_items);
    // FIXME: this is just a simple impl
    // TODO: make better solution in xgd::ComposerHelper later
    // call sub index in _items as oldId
    // <oldId> for atom, line bond, circle and grouped string
    std::vector<size_t> aIds, bIds, cIds, gIds;
    // <bondSideId=oldId+<0,1>+_items.size, bondId=oldId> for bond side points
    std::unordered_map<size_t, size_t> bSidesMap;
    // cast bond id to a new id for bond-from-side id
    auto cast_side1 = [&](const size_t &_bid) -> size_t { return 2 * (_bid + _items.size()); };
    // cast bond id to a new id for bond-to-side id
    auto cast_side2 = [&](const size_t &_bid) -> size_t { return cast_side1(_bid) + 1; };
    // revert bond-side-id to bond id
    auto revert_bond_side = [&](const size_t &_id) -> size_t {
        if (_id >= _items.size()) {
            return (_id - _id % 2) / 2 - _items.size();
        }
        return _id;
    };
    // 0: from; 1: to; 2: not bond side
    auto get_bond_side_type = [&](const size_t &_id) -> int {
        if (_id < _items.size()) { return 2; }
        return _id % 2 != 0;
    };
    // <oldId1,oldId2> means id1 and id2 are impossible to be together
    // notice that we need to insert both {id1,id2} and {id2,id1}
    // e.g. call add_black_list0633, 0022)
    std::unordered_set<std::pair<size_t, size_t>, pair_hash> blackList;
    auto kill_common = [&](const size_t &_i1, const size_t &_i2) {
        blackList.insert({_i1, _i2});
        blackList.insert({_i2, _i1});
    };
    auto is_valid_pair = [&](const size_t &_i1, const size_t &_i2) {
        return blackList.end() == blackList.find({_i1, _i2});
    };
    // 统计键长
    std::unordered_map<size_t, float> bLength;
    auto calc_pts_to_pts = [](const cv::Point2f &_p1, const cv::Point2f &_p2) -> float {
        return std::sqrt(std::pow(_p1.x - _p2.x, 2) + std::pow(_p1.y - _p2.y, 2));
    };
    auto init_bond_side = [&](const size_t &_bid) {
        size_t from = cast_side1(_bid), to = cast_side2(_bid);
        bSidesMap[from] = _bid;
        bSidesMap[to] = _bid;
        // from and to in one bond can't be connected
        kill_common(from, to);
        kill_common(from, _bid);
        kill_common(_bid, to);
        bLength[_bid] = calc_pts_to_pts(_items[_bid].getFrom(), _items[_bid].getTo());
    };
    // 收集id、计算键长
    for (size_t i = 0; i < _items.size(); i++) {
        switch (_items[i].type) {
            case OCRItemType::Element:
                aIds.push_back(i);
                break;
            case OCRItemType::Group:
                gIds.push_back(i);
                break;
            case OCRItemType::Line: {
                bIds.push_back(i);
                init_bond_side(i);
                break;
            }
            case OCRItemType::Circle:
                cIds.push_back(i);
                break;
            default:
                throw std::runtime_error("unhandled OCRItemType in GraphComposer::compose");
        }
    }
    // < <oldId1,{<oldId2, normed distance k>, ...}
    using feat_type = std::pair<size_t, float>;
    std::map<size_t, std::vector<feat_type>> feats;
    auto add_feat = [&](const size_t &_id1, const size_t &_id2, const float &_feat) {
        auto it1 = feats.find(_id1);
        if (feats.end() == it1) {
            feats[_id1] = {{_id2, _feat}};
        } else {
            it1->second.emplace_back(_id2, _feat);
        }
        auto it2 = feats.find(_id2);
        if (feats.end() == it2) {
            feats[_id2] = {{_id1, _feat}};
        } else {
            it2->second.emplace_back(_id1, _feat);
        }
    };
    auto calc_pts_to_rect = [](const cv::Point2f &_p, const cv::Rect2f &_r) -> float {
        float dx = (std::max)((std::max)(_r.x - _p.x, _p.x - (_r.x + _r.width)), 0.f);
        float dy = (std::max)((std::max)(_r.y - _p.y, _p.y - (_r.y + _r.height)), 0.f);
        return std::sqrt(std::pow(dx, 2) + std::pow(dy, 2));
    };
    static const float sAtomBondThresh = 0.5;
    static const float sGroupBondThresh = 0.5;
    static const float sBSideBSideThresh = 0.5;
    auto kill_atom_bond = [&](const size_t &_aid, const size_t &_bid) {
        kill_common(_aid, _bid);
        kill_common(_aid, cast_side1(_bid));
        kill_common(_aid, cast_side2(_bid));
    };
    // 统计元素边框和键端的距离
    for (auto &aid:aIds) {
        auto &ocrAtomItem = _items[aid];
        auto &rect = ocrAtomItem.getRect();
        for (auto &bid:bIds) {
//            if (!is_valid_pair(aid, bid)) { continue; }
            auto &ocrBondItem = _items[bid];
            auto &from = ocrBondItem.getFrom();
            auto &to = ocrBondItem.getTo();
            float length = (std::max)(rect.height, bLength[bid]);
            float d1 = calc_pts_to_rect(from, rect) / length;
            float d2 = calc_pts_to_rect(to, rect) / length;
            if (d1 > sAtomBondThresh && d2 > sAtomBondThresh) {
                kill_atom_bond(aid, bid);
                continue;
            }
            if (d1 < d2) {
                add_feat(aid, cast_side1(bid), d1);
//                feats.insert({{aid, cast_side1(bid)}, d1});
                kill_common(aid, cast_side2(bid));
            } else {
                add_feat(aid, cast_side2(bid), d2);
//                feats.insert({{aid, cast_side2(bid)}, d2});
                kill_common(aid, cast_side1(bid));
            }
        }
    }
    // 统计字符串边框和键端的距离
    for (auto &gid:gIds) {
        auto &ocrGroupItem = _items[gid];
        auto &rect = ocrGroupItem.getRect();
        for (auto &bid:bIds) {
//            if (!is_valid_pair(aid, bid)) { continue; }
            auto &ocrBondItem = _items[bid];
            auto &from = ocrBondItem.getFrom();
            auto &to = ocrBondItem.getTo();
            float length = (std::max)(rect.height, bLength[bid]);
            float d1 = calc_pts_to_rect(from, rect) / length;
            float d2 = calc_pts_to_rect(to, rect) / length;
            if (d1 > sGroupBondThresh && d2 > sGroupBondThresh) {
                kill_atom_bond(gid, bid);
                continue;
            }
            if (d1 < d2) {
                add_feat(gid, cast_side1(bid), d1);
//                feats.insert({{gid, cast_side1(bid)}, d1});
                kill_common(gid, cast_side2(bid));
            } else {
                add_feat(gid, cast_side2(bid), d2);
//                feats.insert({{gid, cast_side2(bid)}, d2});
                kill_common(gid, cast_side1(bid));
            }
        }
    }
    // 统计键端和键端的距离
    for (size_t i = 0; i < bIds.size(); i++) {
        size_t bid1 = bIds[i];
        auto &ocrBondItem1 = _items[bid1];
        size_t fid1 = cast_side1(bid1), tid1 = cast_side2(bid1);
        for (size_t j = i + 1; j < bIds.size(); j++) {
            size_t bid2 = bIds[j];
//            if (!is_valid_pair(bid1, bid2)) { continue; }
            auto &ocrBondItem2 = _items[bid2];
            size_t fid2 = cast_side1(bid2), tid2 = cast_side2(bid2);
            float len = (std::max)(0.01f, (std::max)(bLength[bid1], bLength[bid2]));
            std::map<float, size_t, std::less<float>> distances = {
                    {calc_pts_to_pts(ocrBondItem1.getFrom(), ocrBondItem2.getFrom()) / len, 0},
                    {calc_pts_to_pts(ocrBondItem1.getFrom(), ocrBondItem2.getTo()) / len,   1},
                    {calc_pts_to_pts(ocrBondItem1.getTo(), ocrBondItem2.getFrom()) / len,   2},
                    {calc_pts_to_pts(ocrBondItem1.getTo(), ocrBondItem2.getTo()) / len,     3}
            };
            auto first = distances.begin();
            if (first->first > sBSideBSideThresh) {
                kill_common(bid1, bid2);
                kill_common(fid1, fid2);
                kill_common(fid1, tid2);
                kill_common(tid1, fid2);
                kill_common(tid1, tid2);
                continue;
            }
            for (auto&[feat, id]:distances) {
//                qDebug() << "[b-b distance]\t" << feat << id;
            }
//            qDebug() << "[b-b distance]\t" << "**************************";
            switch (first->second) {
                case 0:
                    add_feat(fid1, fid2, first->first);
//                    feats.insert({{fid1, fid2}, first->first});
                    kill_common(fid1, tid2);
                    kill_common(tid1, fid2);
                    kill_common(tid1, tid2);
                    break;
                case 1:
                    kill_common(fid1, fid2);
                    add_feat(fid1, tid2, first->first);
//                    feats.insert({{fid1, tid2}, first->first});
                    kill_common(tid1, fid2);
                    kill_common(tid1, tid2);
                    break;
                case 2:
                    kill_common(fid1, fid2);
                    kill_common(fid1, tid2);
                    add_feat(tid1, fid2, first->first);
//                    feats.insert({{tid1, fid2}, first->first});
                    kill_common(tid1, tid2);
                    break;
                default:
                    kill_common(fid1, fid2);
                    kill_common(fid1, tid2);
                    kill_common(tid1, fid2);
                    add_feat(tid1, tid2, first->first);
//                    feats.insert({{tid1, tid2}, first->first});
                    break;
            }
        }
    }

    // 决策过程
    // Input: 候选关系对 {<id1,id2,distance,(maybe)feature_map>, ......}
    // Output: 实际关系对 {<id1,id2>, ......}
    qDebug() << "blackList.size()=" << blackList.size();
    qDebug() << "feats.size()=" << feats.size();
    for (auto&[id, vec]:feats) {
        std::sort(vec.begin(), vec.end(), [](const feat_type &_a, const feat_type &_b) -> bool {
            return _a.second < _b.second;
        });
    }
    QString logBuffer = "composer log:\n";
    for (auto&[id, vec]:feats) {
        int type = get_bond_side_type(id);
        if (type == 0) {
            size_t rd = revert_bond_side(id);
            auto &item = _items[rd];
            logBuffer.append(QString("@bond %1 {%2}.from:").arg(rd).arg(item.getText().c_str()));
        } else if (type == 1) {
            size_t rd = revert_bond_side(id);
            auto &item = _items[rd];
            logBuffer.append(QString("@bond %1 {%2}.to:").arg(rd).arg(item.getText().c_str()));
        } else {
            auto &item = _items[id];
            logBuffer.append(QString("@atom %1 {%2}.mode:").arg(id).arg(item.getText().c_str()));
        }
        logBuffer.append("\n");
        for (auto&[nebId, feat]:vec) {
            int nebType = get_bond_side_type(nebId);
            if (nebType == 0) {
                size_t nebRd = revert_bond_side(nebId);
                auto &item = _items[nebRd];
                logBuffer.append(QString("[bond %0 {%1}.from: %2]\t").arg(
                        nebRd).arg(item.getText().c_str()).arg(QString::number(feat, 'f', 3)));
            } else if (nebType == 1) {
                size_t nebRd = revert_bond_side(nebId);
                auto &item = _items[nebRd];
                logBuffer.append(QString("[bond %0 {%1}.to: %2]\t").arg(
                        nebRd).arg(item.getText().c_str()).arg(QString::number(feat, 'f', 3)));
            } else {
                auto &item = _items[nebId];
                logBuffer.append(QString("[atom %0 {%1}.node: %2]\t").arg(
                        nebId).arg(item.getText().c_str()).arg(QString::number(feat, 'f', 3)));
            }
        }
        logBuffer.append("\n-------------------------------------\n");
    }
    qDebug() << logBuffer.toStdString().c_str();
    // 构造集合节点
    // <集合id，<{item1,item2,...}, center>>
    // 元素中心集合的特点：单中心，集合item数量上限由元素决定
    // 字串中心集合的特点：多中心，不考虑集合item数量上限
    // 键端中心集合的特点：需要初始化中心，中心动态变化，最终的中心是键端多边形的重心，集合item数量上限由碳原子决定
    // 表示方式：aNodeMap, gNodeMap使用itemId作为集合id，bNodeMap使用>=_items.size()的数作为集合id
    std::unordered_map<size_t, std::unordered_set<size_t>> aNodeMap, gNodeMap, bNodeMap;
    size_t nodeIdx = _items.size();
    // 图元向集合的映射
    // <图元id，集合id>
    std::unordered_map<size_t, size_t> itemToNodeMap;
    auto init_atom_node = [&](const size_t &_aid) {
        itemToNodeMap[_aid] = _aid;
        aNodeMap[_aid] = {};
    };
    auto init_group_node = [&](const size_t &_gid) {
        itemToNodeMap[_gid] = _gid;
        gNodeMap[_gid] = {};
    };
    auto init_bond_side_node = [&](const size_t &_bSideId) {
        itemToNodeMap[_bSideId] = nodeIdx;
        bNodeMap[nodeIdx++] = {_bSideId};
    };
    auto bind_bond_side_node = [&](const size_t &_bSideId1, const size_t &_bSideId2) {
        auto it = itemToNodeMap.find(_bSideId1);
        if (itemToNodeMap.end() == it) { it = itemToNodeMap.find(_bSideId2); }
        if (itemToNodeMap.end() == it) {
            init_bond_side_node(_bSideId1);
            it = itemToNodeMap.find(_bSideId1);
        }
        size_t nodeId = it->second;
        if (nodeId < _items.size()) {
            auto &item = _items[nodeId];
            switch (item.type) {
                case OCRItemType::Element:
                    aNodeMap[nodeId].insert(_bSideId1);
                    aNodeMap[nodeId].insert(_bSideId2);
                    break;
                case OCRItemType::Group:
                    gNodeMap[nodeId].insert(_bSideId1);
                    gNodeMap[nodeId].insert(_bSideId2);
                    break;
                default:
                    break;
            }
        } else {
            bNodeMap[nodeId].insert(_bSideId1);
            bNodeMap[nodeId].insert(_bSideId2);
        }
        itemToNodeMap[_bSideId1] = itemToNodeMap[_bSideId2] = nodeId;
    };
    auto bind_bond_node = [&](const size_t &_bSideId, const size_t &_id) {
        auto it = itemToNodeMap.find(_bSideId);
        size_t rd = revert_bond_side(_id);
        auto &item = _items[rd];
        switch (item.type) {
            case OCRItemType::Element:
                itemToNodeMap[_id] = itemToNodeMap[_bSideId] = _id;
                aNodeMap[_id].insert(_bSideId);
                break;
            case OCRItemType::Group:
                itemToNodeMap[_id] = itemToNodeMap[_bSideId] = _id;
                gNodeMap[_id].insert(_bSideId);
                break;
            case OCRItemType::Line:
                bind_bond_side_node(_bSideId, _id);
                break;
            default:
                break;
        }
    };
    for (auto &aid:aIds) { init_atom_node(aid); }
    for (auto &gid:gIds) { init_group_node(gid); }

    auto handle_bond_side = [&](const size_t &_bSideId) {
        auto it = feats.find(_bSideId);
        if (feats.end() == it) {
            init_bond_side_node(_bSideId);
            return;
        }
        auto &nebVec = it->second;
        // 遍历所有潜在邻居
        for (auto&[nebId, feat]:nebVec) {
            // 通过 blackList 保证键端集合互斥
            if (is_valid_pair(_bSideId, nebId)) {
                kill_common(_bSideId, nebId);
                bind_bond_node(_bSideId, nebId);
            }
        }
    };
    for (auto &bid:bIds) {
        size_t fId = cast_side1(bid);
        size_t tId = cast_side2(bid);
        handle_bond_side(fId);
        handle_bond_side(tId);
    }
    //
    //// ************************************************** ////
    auto mol = std::make_shared<xgd::JMolAdapter>();
    std::unordered_map<size_t, std::shared_ptr<xgd::JAtom>> bondSideAtomMap, bondSideGroupMap;
//    std::unordered_map<size_t, std::shared_ptr<xgd::JResidue>> bondSideResidueMap;

    // 添加元素图元
    for (auto&[aid, itemSet]:aNodeMap) {
        auto &item = _items[aid];
        auto pos = item.getCenter();
        auto atom = mol->addAtom(item.getElement(), pos.x, pos.y);
        auto &rect = item.getRect();
//        atom->x0 = rect.x;
//        atom->y0 = rect.y;
//        atom->x1 = rect.x + rect.width;
//        atom->y1 = rect.y + rect.height;
        for (auto &bSideId:itemSet) {
            bondSideAtomMap[bSideId] = atom;
        }
    }
    // 添加字串图元
    for (auto&[gid, itemSet]:gNodeMap) {
        auto &item = _items[gid];
        auto &rect = item.getRect();
//        auto pos = item.getCenter();
//        cv::Point2f pos(0, 0);
//        for (auto &bSideId:itemSet) {
//            int type = get_bond_side_type(bSideId);
//            size_t bid = revert_bond_side(bSideId);
//            auto &item = _items[bid];
//            if (type == 0) {
//                pos += item.getFrom();
//            } else if (type == 1) {
//                pos += item.getTo();
//            } else {
//                qDebug() << "error: type !=0 && type != 1@" << __FILE__ << "@" << __LINE__;
//            }
//        }
//        if (!itemSet.empty()) { pos /= static_cast<float>(itemSet.size()); }
        auto superAtom = mol->addSuperAtom(item.getText(), rect.x, rect.y,
                                           rect.x + rect.width, rect.y + rect.height);
        for (auto &bSideId:itemSet) {
            bondSideGroupMap[bSideId] = superAtom;
        }
    }
    // 收集键图元的起始原子和结束原子
    for (auto&[_, itemSet]:bNodeMap) {
        cv::Point2f pos(0, 0);
        for (auto &bSideId:itemSet) {
            int type = get_bond_side_type(bSideId);
            size_t bid = revert_bond_side(bSideId);
            auto &item = _items[bid];
            if (type == 0) {
                pos += item.getFrom();
            } else if (type == 1) {
                pos += item.getTo();
            } else {
                qDebug() << "error: type !=0 && type != 1@" << __FILE__ << "@" << __LINE__;
            }
        }
        if (!itemSet.empty()) { pos /= static_cast<float>(itemSet.size()); }
        auto atom = mol->addAtom(ElementType::C, pos.x, pos.y);
        atom->setImplicit();
        for (auto &bSideId:itemSet) {
            bondSideAtomMap[bSideId] = atom;
        }
    }
    auto get_side_atom = [&](const size_t &_bSideId) -> std::shared_ptr<xgd::JAtom> {
        auto it = bondSideAtomMap.find(_bSideId);
        if (bondSideAtomMap.end() == it) {
            return nullptr;
        } else {
            return it->second;
        }
    };
    auto get_side_group = [&](const size_t &_bSideId) -> std::shared_ptr<xgd::JAtom> {
        auto it = bondSideGroupMap.find(_bSideId);
        if (bondSideGroupMap.end() == it) {
            return nullptr;
        } else {
            return it->second;
        }
    };
    // 添加键图元
    for (auto &bid:bIds) {
        auto &item = _items[bid];
        size_t fId = cast_side1(bid);
        size_t tId = cast_side2(bid);
        float offset1 = 0.5, offset2 = 0.5;
        auto from = get_side_atom(fId);
        if (!from) {
            from = get_side_group(fId);
            if (from) {
                // 按照字符串计算键端偏移量
                float width = from->x1 - from->x0;
                float offset = width ? (item.getFrom().x - from->x0) / width : 0.5;
                offset1 = (std::min)(1.f, (std::max)(0.f, offset));
            }
        }
        auto to = get_side_atom(tId);
        if (!to) {
            to = get_side_group(tId);
            if (to) {
                // 按照字符串计算键端偏移量
                float width = to->x1 - to->x0;
                float offset = width ? (item.getTo().x - to->x0) / width : 0.5;
                offset2 = (std::min)(1.f, (std::max)(0.f, offset));
            }
        }
        if (from && to) {
            mol->addBond(from, to, item.getBondType(), offset1, offset2);
        } else {
            qDebug() << "error: not from && to@" << __FILE__ << "@" << __LINE__;
        }
    }
    return mol;
}
