#include "synthesizer.h"

#include "geoutil.h"
#include <QLabel>

Synthesizer::Synthesizer() {
}

const char *Synthesizer::label2name(int label) {
    switch (label) {
        case 0:
            return "DashBond";
        case 1:
            return "SingleBond";
        case 2:
            return "DoubleBond";
        case 3:
            return "TripleBond";
        case 4:
            return "SolidBond";
        case 5:
            return "C";
        case 6:
            return "H";
        case 7:
            return "O";
        case 8:
            return "N";
        case 9:
            return "P";
        case 10:
            return "S";
        default:
            assert(false);
            return "C";
    }
}

void Synthesizer::collect_XgdsImplement(
        std::vector<int> &aids,
        const std::vector<int> &bids,
        std::vector<int> &aindices,
        const std::vector<int> &bindices,
        std::vector<QPointF> &aposes,
        const std::vector<QLineF> &bposes,
        std::vector<std::vector<int>> &cons,
        const cv::Mat &frame,
        const std::vector<RectObj> &rects,
        ItemTopo &items
) {
    qreal maxDistance;
    auto checkExplicitAtom = [&](int i) {
        maxDistance = StdAtomTextAvgWidth;
        const QPointF &a = aposes[i];
        qreal dis = std::numeric_limits<qreal>::max();
        int index = 0;
        for (size_t j = 0; j < bposes.size(); j++) {
            QPointF &&from = bposes[j].p1();
            QPointF &&to = bposes[j].p2();
            qreal distance_a_fromside = GeoUtil::getDistance(from, a),
                    distance_a_toside = GeoUtil::getDistance(to, a);
            if (distance_a_fromside < maxDistance) {
                if (cons[j][1] == -1)
                    cons[j][1] = aids[i];
                else if (GeoUtil::getDistance(from, aposes[cons[j][1]]) > distance_a_fromside)
                    cons[j][1] = aids[i];
            }
            if (distance_a_toside < maxDistance) {
                if (cons[j][2] == -1)
                    cons[j][2] = aids[i];
                else if (GeoUtil::getDistance(to, aposes[cons[j][2]]) > distance_a_toside)
                    cons[j][2] = aids[i];
            }
        }
    };
    ///////////////////////////////////////////////////////////////
    //为显示写出的原子寻找键
    for (size_t i = 0; i < aids.size(); i++) {
        checkExplicitAtom(i);
    }
    int nextStart = aids.size();
    ///////////////////////////////////////////////////////////////
    //添加新原子
    std::vector<std::vector<std::vector<qreal>>> fuck;
    size_t len = bids.size();
    fuck.resize(len);
    for (auto &i : fuck) {
        i.resize(len);        //len*len矩阵
    }
    for (size_t i = 0; i < len; i++) {
        QPointF &&from = bposes[i].p1();
        QPointF &&to = bposes[i].p2();
        for (size_t j = i + 1; j < len; j++) {
            QPointF &&_from = bposes[j].p1();
            QPointF &&_to = bposes[j].p2();
            fuck[i][j] = {
                    GeoUtil::getDistance(from, _from),
                    GeoUtil::getDistance(from, _to),
                    GeoUtil::getDistance(to, _from),
                    GeoUtil::getDistance(to, _to)
            };
        }
    }
    std::vector<QPointF> ps;
    maxDistance = StdBestDetectWidth;
    for (size_t i = 0; i < len; i++) {
        for (size_t j = i + 1; j < len; j++) {
            for (size_t k = 0; k < 4; k++) {
                if (cons[i][1 + k / 2] != -1 || cons[j][1 + k % 2] != -1)
                    continue;
                if (fuck[i][j][k] < maxDistance) {
                    switch (k) {
                        case 0:
                            ps.push_back(bposes[i].p1());
                            ps.push_back(bposes[j].p1());
                            break;
                        case 1:
                            ps.push_back(bposes[i].p1());
                            ps.push_back(bposes[j].p2());
                            break;
                        case 2:
                            ps.push_back(bposes[i].p2());
                            ps.push_back(bposes[j].p1());
                            break;
                        default:
                            ps.push_back(bposes[i].p2());
                            ps.push_back(bposes[j].p2());
                            //ps.push_back((bposes[i].p2() + bposes[j].p2()) / 2);
                            break;
                    }
                }
            }
        }
    }
    maxDistance = StdBestDetectWidth;
    std::vector<bool> IsDisabled;
    IsDisabled.resize(ps.size(), false);
    for (size_t i = 0; i < ps.size(); i++) {
        if (IsDisabled[i])continue;
        for (size_t j = i + 1; j < ps.size(); j++) {
            if (IsDisabled[j])continue;
            if (GeoUtil::getDistance(ps[i], ps[j]) < maxDistance) {
                ps[i] = (ps[i] + ps[j]) / 2;
                IsDisabled[j] = true;
            }
        }
    }
    for (size_t i = 0; i < ps.size(); i++) {
        for (size_t j = 0; j < aindices.size(); j++) {
            qreal x, y, w, h;
            x = rects[aindices[j]].outline.x();
            y = rects[aindices[j]].outline.y();
            w = rects[aindices[j]].outline.width();
            h = rects[aindices[j]].outline.height();
            x -= w / 5.0;
            y -= h / 5.0;
            w *= 7.0 / 5.0;
            h *= 7.0 / 5.0;
            if (QRectF(x, y, w, h).contains(ps[i])) {
                IsDisabled[i] = true;
                break;
            }
        }
    }
    for (size_t i = 0; i < ps.size(); i++) {
        if (!IsDisabled[i]) {
            aids.push_back(items.insertAtom("Atom", "C", ps[i]));
            //aindices.push_back(i);
            aposes.push_back(ps[i]);
        }
    }
    ///////////////////////////////////////////////////////////////
    //为隐式碳原子寻找键
    for (size_t i = nextStart; i < aids.size(); i++) {
        checkExplicitAtom(i);
    }
    ///////////////////////////////////////////////////////////////
    //添加边缘隐式碳原子
    for (size_t i = 0; i < cons.size(); i++) {
        if (cons[i][1] == -1) {
            int id = items.insertAtom("Atom", "C", bposes[i].p1());
            cons[i][1] = id;
            aids.push_back(id);
            //aindices.push_back(i);
            //aposes.push_back(ps[i]);
        }
        if (cons[i][2] == -1) {
            int id = items.insertAtom("Atom", "C", bposes[i].p2());
            cons[i][2] = id;
            aids.push_back(id);
        }
    }
    ///////////////////////////////////////////////////////////////
    //设置items
    for (auto &i : cons) {
        items.setFromTo(i[0], i[1], i[2]);
    }
}

bool Synthesizer::IsAtom(int label) {
    return (label >= 5);
}

void Synthesizer::collect(
        const cv::Mat &frame,
        const std::vector<RectObj> &rects,
        ItemTopo &items
) {
    items.clear();
    std::vector<int> aids, bids,            // ID
    aindices, bindices;                    // 在rects的索引
    std::vector<QPointF> aposes;            // 位置
    std::vector<QLineF> bposes;                // 向量
    for (size_t i = 0; i < rects.size(); i++) {
        if (IsAtom(rects[i].label)) {
            aids.push_back(items.insertAtom("Atom", label2name(rects[i].label), rects[i].outline.center()));
            aindices.push_back(i);
            aposes.push_back(rects[i].outline.center());
        } else {
            bids.push_back(items.insertBond(label2name(rects[i].label)));
            bindices.push_back(i);
            bposes.push_back(CVUtil::getLineDirection(frame, rects[i].outline));
        }
    }
    std::vector<std::vector<int>> cons;    //拓扑连接，<键id，from原子id，to原子id>
    cons.resize(bids.size(), {-1, -1, -1});
    for (size_t i = 0; i < cons.size(); i++) {
        cons[i][0] = bids[i];
    }
    //问题抽象：输入aids、bids、aindices、bindices、aposes、bposes，参考rects，修正items
    //输出：三元组序列cons，记录连接方式
    collect_XgdsImplement(aids, bids, aindices, bindices, aposes, bposes, cons, frame, rects, items);
}
