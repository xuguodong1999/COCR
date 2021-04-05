#include "graph_composer.hpp"
#include "composer/handler.hpp"
#include "jmol_adapter.hpp"
#include <QDebug>
#include <map>

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
    for (auto&[id, vec]:feats) {
        int type = get_bond_side_type(id);
        if (type == 0) {
            size_t rd = revert_bond_side(id);
            auto &item = _items[rd];
            qDebug() << "*bond" << rd << item.getText().c_str() << "from:";
        } else if (type == 1) {
            size_t rd = revert_bond_side(id);
            auto &item = _items[rd];
            qDebug() << "*bond" << rd << item.getText().c_str() << "to:";
        } else {
            auto &item = _items[id];
            qDebug() << "*strg" << id << item.getText().c_str();
        }
        QString info;
        for (auto&[nebId, feat]:vec) {
            int nebType = get_bond_side_type(nebId);
            if (nebType == 0) {
                size_t nebRd = revert_bond_side(nebId);
                auto &item = _items[nebRd];
                info.append(QString("+bond %0 %1 from: %2; ").arg(nebRd).arg(item.getText().c_str()).arg(feat));
            } else if (nebType == 1) {
                size_t nebRd = revert_bond_side(nebId);
                auto &item = _items[nebRd];
                info.append(QString("+bond %0 %1 to: %2; ").arg(nebRd).arg(item.getText().c_str()).arg(feat));
            } else {
                auto &item = _items[nebId];
                info.append(QString("+strg %0 %1: %2; ").arg(nebId).arg(item.getText().c_str()).arg(feat));
            }
        }
        qDebug() << info.toStdString().c_str();
        qDebug() << "-------------------------------------";
    }
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
        itemToNodeMap[_aid] = nodeIdx;
        aNodeMap[_aid] = {};
    };
    auto init_group_node = [&](const size_t &_gid) {
        itemToNodeMap[_gid] = nodeIdx;
        gNodeMap[_gid] = {};
    };
    for (auto &aid:aIds) { init_atom_node(aid); }
    for (auto &gid:gIds) { init_group_node(gid); }

    // 通过 blackList 保证键端集合互斥


    //// ************************************************** ////
    auto mol = std::make_shared<xgd::JMolAdapter>();
    std::unordered_map<size_t, std::shared_ptr<xgd::JAtom>> bondSideAtomMap;
    std::unordered_map<size_t, std::shared_ptr<xgd::JResidue>> bondSideResidueMap;
    // 添加元素图元
    for (auto&[aid, itemSet]:aNodeMap) {
        auto &item = _items[aid];
        auto pos = item.getCenter();
        auto atom = mol->addAtom(item.getElement(), pos.x, pos.y);
        for (auto &bSideId:itemSet) {
            bondSideAtomMap[bSideId] = atom;
        }
    }
    // 添加字串图元
    for (auto&[gid, itemSet]:gNodeMap) {
        auto &item = _items[gid];
        auto pos = item.getCenter();
        // FIXME: 先指定砹元素代表残基，残基这一块还没有实现
        auto atom = mol->addAtom(ElementType::At, pos.x, pos.y);
        for (auto &bSideId:itemSet) {
            bondSideAtomMap[bSideId] = atom;
        }
    }
    // 收集键图元的起始原子和结束原子
    for (auto&[_, itemSet]:bNodeMap) {
        cv::Point2f pos(0, 0);
        for (auto &bSideId:itemSet) {
            int type = revert_bond_side(bSideId);
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
    // 添加键图元
    for (auto &bid:bIds) {
        auto &item = _items[bid];
        size_t fId = cast_side1(bid);
        size_t tId = cast_side2(bid);
        auto from = get_side_atom(fId);
        auto to = get_side_atom(tId);
        if (from && to) {
            mol->addBond(from, to, item.getBondType());
        } else {
            qDebug() << "error: not from && to@" << __FILE__ << "@" << __LINE__;
        }
    }
    return mol;
}
