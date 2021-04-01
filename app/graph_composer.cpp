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
    // < <oldId1,oldId2>, normed distance k>
    std::map<std::pair<size_t, size_t>, float> feats;
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
                feats.insert({{aid, cast_side1(bid)}, d1});
                kill_common(aid, cast_side2(bid));
            } else {
                feats.insert({{aid, cast_side2(bid)}, d2});
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
                feats.insert({{gid, cast_side1(bid)}, d1});
                kill_common(gid, cast_side2(bid));
            } else {
                feats.insert({{gid, cast_side2(bid)}, d2});
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
                qDebug() << "[b-b distance]\t" << feat << id;
            }
            qDebug() << "[b-b distance]\t" << "**************************";
            switch (first->second) {
                case 0:
                    feats.insert({{fid1, fid2}, first->first});
                    kill_common(fid1, tid2);
                    kill_common(tid1, fid2);
                    kill_common(tid1, tid2);
                    break;
                case 1:
                    kill_common(fid1, fid2);
                    feats.insert({{fid1, tid2}, first->first});
                    kill_common(tid1, fid2);
                    kill_common(tid1, tid2);
                    break;
                case 2:
                    kill_common(fid1, fid2);
                    kill_common(fid1, tid2);
                    feats.insert({{tid1, fid2}, first->first});
                    kill_common(tid1, tid2);
                    break;
                default:
                    kill_common(fid1, fid2);
                    kill_common(fid1, tid2);
                    kill_common(tid1, fid2);
                    feats.insert({{tid1, tid2}, first->first});
                    break;
            }
        }
    }

    // 决策过程
    // Input: 候选关系对 {<id1,id2,distance,(maybe)feature_map>, ......}
    // Output: 实际关系对 {<id1,id2>, ......}
    qDebug() << "blackList.size()=" << blackList.size();
    qDebug() << "feats.size()=" << feats.size();
    for (auto&[grp, val]:feats) {
        auto[id1, id2]=grp;
        id1 = revert_bond_side(id1);
        id2 = revert_bond_side(id2);
        auto &item1 = _items[id1], &item2 = _items[id2];
        qDebug() << "[val]\t" << val << "\tlink" << item1.getText().c_str() << "and" << item2.getText().c_str();
    }

    auto mol = std::make_shared<xgd::JMolAdapter>();
    return mol;
}
