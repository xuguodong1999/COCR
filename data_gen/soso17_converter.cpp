#include "soso17_converter.hpp"
#include "opencv_util.hpp"
#include "std_util.hpp"
#include "colors.hpp"
#include "mol_util.hpp"
#include "mol_op.hpp"

#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <map>
#include <vector>
#include <variant>
#include <numeric>
#include <unordered_map>

extern std::shared_ptr<MolUtil> molUtil;

std::vector<std::string> CLASSES = {
        "Br", "O", "I", "S", "H", "N", "C", "B",
        "-", "--", "-+", "=", "F", "#", "Cl", "P", "[o]"};

ElementType convertLabelToElementType(const int &_label) {
    switch (_label) {
        case 0:
            return ElementType::Br;
        case 1:
            return ElementType::O;
        case 2:
            return ElementType::I;
        case 3:
            return ElementType::S;
        case 4:
            return ElementType::H;
        case 5:
            return ElementType::N;
        case 6:
            return ElementType::C;
        case 7:
            return ElementType::B;
        case 12:
            return ElementType::F;
        case 14:
            return ElementType::Cl;
        case 15:
            return ElementType::P;
        default: {
            std::cerr << "error label " << _label << "as atom type" << std::endl;
            return ElementType::None;
        }
    }
}

JBondType convertLabelToBondType(const int &_label) {
    switch (_label) {
        case 8:
            return JBondType::SingleBond;
        case 11:
            return JBondType::DoubleBond;
        case 13:
            return JBondType::TripleBond;
        case 9:
            return JBondType::DashWedgeBond;
        case 10:
            return JBondType::SolidWedgeBond;
        case 16:
            return JBondType::CircleBond;
        default: {
            std::cerr << "error label " << _label << "as bond type" << std::endl;
            return JBondType::NoneBond;
        }
    }
}

bool isBondOrStr(const int &_label) {
    switch (_label) {
        case 0 :
        case 1:
        case 2:
        case 3 :
        case 4 :
        case 5 :
        case 6 :
        case 7 :
        case 12 :
        case 14 :
        case 15: {
            return false;
        }
        default: {
            return true;
        }
    }
}


SOSO17Converter::SOSO17Converter() : MolHolder(nullptr) {

}

void SOSO17Converter::accept(const cv::Mat &_img,
                             const std::vector<YoloObject> &_objs) {
    clear();
    cv::cvtColor(_img, imgGray, cv::COLOR_BGR2GRAY);
    for (auto &obj:_objs) {
        if (isBondOrStr(obj.label)) {
            // 是键
            auto bondType = convertLabelToBondType(obj.label);
            if (JBondType::CircleBond == bondType) {
                // 是圆
                circles.emplace_back(obj.x + obj.w / 2,
                                     obj.y + obj.h / 2,
                                     (obj.w + obj.h) / 4);
            } else {
                // 是线
                lines.emplace_back(obj.x, obj.y, obj.w, obj.h, bondType);
                lines.back().predFromTo(imgGray);
            }
        } else {
            // 是字符
            auto elementType = convertLabelToElementType(obj.label);
            chars.emplace_back(obj.x, obj.y, obj.w, obj.h, elementType);
        }
    }
    then();
}

void SOSO17Converter::clear() {
    chars.clear();
    lines.clear();
    circles.clear();
    imgGray.release();
    reset(std::make_shared<JMol>());
}


void SOSO17Converter::then() {
    using quota_type = std::tuple<float, size_t, ItemType, size_t, ItemType>;
    std::vector<quota_type> quotas;
    for (size_t i = 0; i < chars.size(); i++) {
        auto &a = chars[i];
        float w_2 = a.w / 2, h_2 = a.h / 2;
        for (size_t j = 0; j < lines.size(); j++) {
            auto &b = lines[j];
            auto calc_distance = [&](const cv::Point2f &_p) -> float {
                float dx = std::fabs(_p.x - a.center.x);
                float dy = std::fabs(_p.y - a.center.y);
                float dis = std::sqrt(dx * dx + dy * dy);
                if (dx < w_2 && dy < h_2) {//skip
                } else if (dx < w_2) {//dy>h_2
                    dis *= (dy - h_2) / dy;
                } else if (dy < h_2) {//dx>w_2
                    dis *= (dx - w_2) / dx;
                } else {// 近似
                    dis -= std::max(h_2, w_2);
                }
                return dis / b.length;
            };
            float q1 = calc_distance(b.from), q2 = calc_distance(b.to);
            if (q1 < sQuotaThresh) {
                quotas.emplace_back(
                        q1, i, ItemType::ExplicitAtom, j, ItemType::BondFrom);
            }
            if (q2 < sQuotaThresh) {
                quotas.emplace_back(
                        q2, i, ItemType::ExplicitAtom, j, ItemType::BondTo);
            }
        }
    }
    for (size_t i = 0; i < lines.size(); i++) {
        auto &b1 = lines[i];
        for (size_t j = i + 1; j < lines.size(); j++) {
            auto &b2 = lines[j];
            float avgLen = (b1.length + b2.length) / 2 / sBondExpandThresh;
            float q1 = getDistance2D(b1.from, b2.from) / avgLen;
            float q2 = getDistance2D(b1.from, b2.to) / avgLen;
            float q3 = getDistance2D(b1.to, b2.from) / avgLen;
            float q4 = getDistance2D(b1.to, b2.to) / avgLen;
            if (q1 < sQuotaThresh) {
                quotas.emplace_back(
                        q1, i, ItemType::BondFrom, j, ItemType::BondFrom);
            }
            if (q2 < sQuotaThresh) {
                quotas.emplace_back(
                        q2, i, ItemType::BondFrom, j, ItemType::BondTo);
            }
            if (q3 < sQuotaThresh) {
                quotas.emplace_back(
                        q3, i, ItemType::BondTo, j, ItemType::BondFrom);
            }
            if (q4 < sQuotaThresh) {
                quotas.emplace_back(
                        q4, i, ItemType::BondTo, j, ItemType::BondTo);
            }
        }
    }
    std::sort(quotas.begin(), quotas.end(), [](
            const quota_type &_a, const quota_type &_b) {
        return std::get<0>(_a) < std::get<0>(_b);
    });

    // 通过形式化的抽象总结构造分子树
    struct Item {
        enum TmpType {
            AA, BB, CC
        } t;
//        size_t id;
        size_t belonging;
    };
    using S = std::unordered_set<size_t>;
    std::vector<Item> m_all;
    std::vector<S> m_sets;
    auto solve_soso17 = [&]() {
        using F = std::pair<size_t, size_t>;
        std::map<std::pair<size_t, ItemType>, size_t> m_caster;
        std::vector<F> m_feats;
        for (size_t i = 0; i < chars.size(); i++) {
            auto &charItem = chars[i];
            Item itemA;
            itemA.t = Item::AA;
//        itemA.id = idx++;
            itemA.belonging = i;
            m_all.push_back(itemA);
            m_caster[{i, ItemType::ExplicitAtom}] = m_all.size() - 1;
        }
        for (size_t i = 0; i < lines.size(); i++) {
            auto &line = lines[i];
            Item itemB;
            itemB.t = Item::BB;
            itemB.belonging = i;
//        itemB.id = idx++;
            m_all.push_back(itemB);
            m_caster[{i, ItemType::BondFrom}] = m_all.size() - 1;
            itemB.t = Item::CC;
//        itemB.id = idx++;
            m_all.push_back(itemB);
            m_caster[{i, ItemType::BondTo}] = m_all.size() - 1;
        }
        for (auto &[feat, id1, type1, id2, type2]:quotas) {
            m_feats.emplace_back(m_caster[{id1, type1}], m_caster[{id2, type2}]);
        }
        std::unordered_map<size_t, size_t> valSet;// <id，集合下标>
        auto insert_for_01 = [&](const decltype(valSet.end()) &_itr_none,
                                 const decltype(valSet.end()) &_itr_exist,
                                 const size_t &_none, const size_t &_exist) {
            // _exist 已经在一个集合里了
            auto &curSet = m_sets[_itr_exist->second];
            // 验证 _none 能不能被插入
            if (m_all[_none].t == Item::AA) {
                bool existA = false;
                for (auto &id:curSet) {
                    if (m_all[id].t == Item::AA) {
                        existA = true;
                        break;
                    }
                }
                if (!existA) {
                    // 插入
                    valSet[_none] = _itr_exist->second;
                    curSet.insert(_none);
                }
            } else {// BB or CC
                bool existSameBond = false;
                for (auto &id:curSet) {
                    if (m_all[id].t != Item::AA &&
                        m_all[id].belonging == m_all[_none].belonging) {
                        existSameBond = true;
                        break;
                    }
                }
                if (!existSameBond) {
                    // 插入
                    valSet[_none] = _itr_exist->second;
                    curSet.insert(_none);
                }
            }
        };
        for (auto&[id1, id2]:m_feats) {
            auto itr1 = valSet.find(id1);
            auto itr2 = valSet.find(id2);
            if (itr1 == valSet.end() && itr2 == valSet.end()) {
                // FIXME: 关系对属于一个已知聚类
                // 这个留在最后打一个补丁，对象是所有仅包含两个键端的聚类
                // 造一个新集合
                m_sets.push_back(S());
                auto &curSet = m_sets.back();
                // 无条件插入
                curSet.insert(id1);
                curSet.insert(id2);
                // 标记在哪个集合
                valSet[id1] = valSet[id2] = m_sets.size() - 1;
            } else if (itr1 == valSet.end()) {
                insert_for_01(itr1, itr2, id1, id2);
            } else if (itr2 == valSet.end()) {
                insert_for_01(itr2, itr1, id2, id1);
            } else {
                // 需要合并吗
            }
        }
        for (size_t i = 0; i < m_all.size(); i++) {
            if (notExist(valSet, i)) {
                // 造一个新集合
                m_sets.push_back(S());
                auto &curSet = m_sets.back();
                // 无条件插入
                curSet.insert(i);
                // 标记在哪个集合
                valSet[i] = m_sets.size() - 1;
            }
        }
        auto val_pts = [](const std::vector<cv::Point2f> &_pts, const float &_d) {
            std::vector<float> dis = {
                    getDistance2D(_pts[0], _pts[1]),
                    getDistance2D(_pts[0], _pts[2]),
                    getDistance2D(_pts[0], _pts[3]),
                    getDistance2D(_pts[1], _pts[2]),
                    getDistance2D(_pts[1], _pts[3]),
                    getDistance2D(_pts[2], _pts[3])
            };
            std::sort(dis.begin(), dis.end(), std::greater<float>());
            return dis[0] * sGroupPtsThresh < _d;
        };
        for (size_t i = 0; i < m_sets.size(); i++) {
            auto &m_set = m_sets[i];
            if (m_set.size() != 2)continue;
            bool isTwoBondSide = true;
            std::unordered_set<size_t> belongingSet;
            for (auto &id:m_set) {
                belongingSet.insert(m_all[id].belonging);
                if (Item::AA == m_all[id].t) {
                    isTwoBondSide = false;
                    break;
                }
            }
            if (!isTwoBondSide)continue;
            for (size_t j = i + 1; j < m_sets.size(); j++) {
                if (m_sets[j].size() != 2)continue;
                isTwoBondSide = true;
                for (auto &id:m_sets[j]) {
                    if (Item::AA == m_all[id].t ||
                        !notExist(belongingSet, m_all[id].belonging)) {
                        isTwoBondSide = false;
                        break;
                    }
                }
                if (!isTwoBondSide)continue;
                // 比较两个集合<i,j>的距离
                std::vector<cv::Point2f> pts;
                std::vector<float> lens;
                for (auto &id:m_sets[j]) {
                    if (Item::BB == m_all[id].t) {
                        pts.push_back(lines[m_all[id].belonging].from);
                        lens.push_back(lines[m_all[id].belonging].length);
                    } else if (Item::CC == m_all[id].t) {
                        pts.push_back(lines[m_all[id].belonging].to);
                        lens.push_back(lines[m_all[id].belonging].length);
                    }
                }
                for (auto &id:m_set) {
                    if (Item::BB == m_all[id].t) {
                        pts.push_back(lines[m_all[id].belonging].from);
                        lens.push_back(lines[m_all[id].belonging].length);
                    } else if (Item::CC == m_all[id].t) {
                        pts.push_back(lines[m_all[id].belonging].to);
                        lens.push_back(lines[m_all[id].belonging].length);
                    }
                }
                if (val_pts(pts, std::accumulate(
                        lens.begin(), lens.end(), 0) / lens.size())) {
                    for (auto &id:m_sets[j])m_set.insert(id);
                    m_sets.erase(m_sets.begin() + j);
                    break;// 合过一次，不可能再合一次
                }
            }

        }
    };
    solve_soso17();

    if (!mol)mol = std::make_shared<JMol>();
    mol->clear();
    // 新建原子
    std::vector<size_t> a2a(m_sets.size());// <集合索引，mol中的原子id>
    for (size_t i = 0; i < m_sets.size(); i++) {
        auto &m_set = m_sets[i];
        bool containAtom = false;
        std::shared_ptr<JAtom> atom;
        cv::Point2f p;
        for (auto &id:m_set) {
            if (m_all[id].t == Item::AA) {
                containAtom = true;
                size_t aid = m_all[id].belonging;
                p = chars[aid].center;
                atom = mol->addAtom(chars[aid].elementType);
                break;
            }
        }
        if (!containAtom) {
            for (auto &id:m_set) {
                if (m_all[id].t == Item::BB) {
                    p += lines[m_all[id].belonging].from;
                } else {
                    p += lines[m_all[id].belonging].to;
                }
            }
            p /= (float) m_set.size();
            atom = mol->addAtom(ElementType::C);
        }
//        std::cout<<"p="<<p<<std::endl;
        atom->x = p.x;
        atom->y = p.y;
        a2a[i] = atom->getId();
    }
    // 新建线形键
    std::vector<size_t> bc(lines.size(), 0);
    std::vector<std::pair<size_t, size_t>> ba(lines.size());
    for (size_t i = 0; i < m_sets.size(); i++) {
        auto &m_set = m_sets[i];
        for (auto &id:m_set) {
            size_t bid = m_all[id].belonging;
            if (m_all[id].t == Item::BB) {
                bc[bid]++;
                ba[bid].first = a2a[i];
            } else if (m_all[id].t == Item::CC) {
                bc[bid]++;
                ba[bid].second = a2a[i];
            }
            if (m_all[id].t != Item::AA && 2 == bc[bid]) {
                mol->addBond(ba[bid].first, ba[bid].second,
                             lines[m_all[id].belonging].bondType);
            }
        }
    }
    if (!circles.empty()) {
        // 处理圆
        auto rings = molUtil->getSSSR(*mol);
        std::cout << "rings.size()=" << rings.size() << std::endl;
        if (rings.empty()) {
            std::cerr << circles.size() << " circles but no rings detected" << std::endl;
            return;
        }
        auto op = std::make_shared<MolOp>(getMol());
        for (auto &ring:rings) {
            if (ring.empty())continue;
//        std::cout<<"ring.size()="<<ring.size()<<std::endl;
            cv::Point2f p(0, 0);
            float r = 0;
            for (auto &id:ring) {
                p += cv::Point2f(mol->getAtomById(id)->x, mol->getAtomById(id)->y);
            }
            p /= (float) ring.size();
            for (auto &id:ring) {
                r += getDistance2D(p, cv::Point2f(
                        mol->getAtomById(id)->x, mol->getAtomById(id)->y));
            }
            r /= ring.size();
            bool needAromatic = false;
            for (auto &circle:circles) {
                if (std::fabs(r - circle.radius) > getDistance2D(p, circle.center)) {
                    needAromatic = true;
                    break;
                }
            }
            std::cout << "needAromatic=" << needAromatic << "\n";
            if (!needAromatic) continue;
            std::unordered_set<size_t> aidSet;
            for (auto &id:ring)aidSet.insert(id);
            std::vector<std::shared_ptr<JBond>> cb, cb2;
            auto modify_bond = [&](const size_t &_bid) -> void {
                auto bond = mol->getBondById(_bid);
                if (notExist(aidSet, bond->getAtomFrom()) ||
                    notExist(aidSet, bond->getAtomTo())) {
                    return;
                }
                cb.push_back(std::move(bond));
            };
            mol->safeTraverseBonds(modify_bond);
            size_t start = 0;
            for (size_t i = 0; i < cb.size(); i++) {
                if (JBondType::DoubleBond == cb[i]->getBondType()) {
                    start = i;
                    break;
                }
            }
            size_t target = cb[start]->getAtomFrom();
            cb2.push_back(cb[start]);
            cb.erase(cb.begin() + start);
            while (!cb.empty()) {
                for (size_t i = 0; i < cb.size(); i++) {
                    if (cb[i]->getAtomFrom() == target) {
                        cb2.push_back(cb[i]);
                        target = cb[i]->getAtomTo();
                        cb.erase(cb.begin() + i);
                        break;
                    } else if (cb[i]->getAtomTo() == target) {
                        cb2.push_back(cb[i]);
                        target = cb[i]->getAtomFrom();
                        cb.erase(cb.begin() + i);
                        break;
                    }
                }
            }
            for (auto &b:cb2) {
//                b->setOrder(JBondType::DelocalizedBond);
                op->markAsDouble(b->getId());
            }
        }
    }

//    std::cout << "m_sets.size()=" << m_sets.size() << std::endl;
//    for (auto &m_set:m_sets) {
//        for (auto &id:m_set) {
//            std::cout << id << ",";
//        }
//        std::cout << "\n*****\n";
//    }
//    size_t breakLine = 0;
//    for (auto &quota:quotas) {
//        std::cout << std::get<0>(quota) << ",";
//        if ((++breakLine) % 5 == 0)std::cout << "\n";
//    }
//    std::cout << std::endl;
//    cv::Mat displayImg;
//    cv::cvtColor(imgGray, displayImg, cv::COLOR_GRAY2BGR);
//    cv::Mat displayImg2(displayImg.rows, displayImg.cols, CV_8UC3,
//                        getScalar(ColorName::rgbWhite));
//    for (size_t i = 0; i < m_all.size(); i++) {
//        auto &m_tar = m_all[i];
//        cv::Point p;
//        float k = 0.7;
//        if (m_tar.t == Item::AA) {
//            p = chars[m_tar.belonging].center;
//        } else if (m_tar.t == Item::BB) {
//            p = 0.7 * lines[m_tar.belonging].from + (1 - k) * lines[m_tar.belonging].to;
//        } else {
//            p = 0.7 * lines[m_tar.belonging].to + (1 - k) * lines[m_tar.belonging].from;
//        }
//        auto info = std::to_string(i);
//        cv::putText(displayImg2, info, p,
//                    1, 1.2, getScalar(ColorName::rgbBlue),
//                    1, cv::LINE_AA, false);
//    }
//    for (auto &line:lines) {
//        cv::rectangle(displayImg, line.rect, getScalar(ColorName::rgbGreen), 1);
//        cv::circle(displayImg, line.from, 4, getScalar(ColorName::rgbBlue), -1);
//        cv::circle(displayImg, line.to, 4, getScalar(ColorName::rgbRed), -1);
//    }
//    cv::imshow("1", displayImg);
//    cv::imshow("2", displayImg2);
//    cv::waitKey(0);
}

void SOSO17Converter::LineBondItem::predFromTo(const cv::Mat &_img) {
    switch (bondType) {
        case JBondType::SingleBond:
        case JBondType::DoubleBond:
        case JBondType::TripleBond: {
            predFromToForLine(_img);
            break;
        }
        default: {
            predFromToForWedge(_img);
            break;
        }
    }
    length = getDistance2D(from, to);
}

void SOSO17Converter::LineBondItem::predFromToForLine(const cv::Mat &_imgGray) {
    float w = rect.width, h = rect.height, x = rect.x, y = rect.y;
    int xmin = (std::max)(0, (int) std::floor(x));
    int ymin = (std::max)(0, (int) std::floor(y));
    int xmax = (std::min)(_imgGray.cols - 1, (int) std::ceil(x + w));
    int ymax = (std::min)(_imgGray.rows - 1, (int) std::ceil(y + h));
    int iw = xmax - xmin + 1, ih = ymax - ymin + 1;
    // 边长足够小 or 足够窄，误差忽略不计，直接取中点作为起始点
    if (iw < sMinSize || ih < sMinSize || w / h > sLineThresh || h / w > sLineThresh) {
        if (w < h) {
            from.x = to.x = x + w / 2;
            from.y = y;
            to.y = y + h;
        } else {
            from.x = x;
            to.x = x + w;
            from.y = to.y = y + h / 2;
        }
        return;
    }
    const int roiSize = 32;
    cv::Mat roiArea;
    cv::resize(_imgGray(cv::Rect2i(xmin, ymin, iw, ih)),
               roiArea, cv::Size(roiSize, roiSize));
    const int step = roiSize / 4;
    const int step2 = 2 * step;
    const int step3 = 3 * step;
    auto get_bw_tl_br = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(0, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step, step, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step3, step3, step, step)))

                + cv::sum(roiArea(cv::Rect2i(0, 0, step2, step2)))
                + cv::sum(roiArea(cv::Rect2i(step2, step2, step2, step2))));
    };
    auto get_bw_bl_tr = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step3, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(0, step3, step, step)))

                + cv::sum(roiArea(cv::Rect2i(step2, 0, step2, step2)))
                + cv::sum(roiArea(cv::Rect2i(0, step2, step2, step2))));
    };
//    std::cout << get_bw_tl_br() << std::endl;
//    std::cout << get_bw_bl_tr() << std::endl;
//    cv::imshow("origin", _imgGray(cv::Rect2i(xmin, ymin, iw, ih)));
//    cv::imshow("roi64", roiArea);
//    cv::waitKey(0);
    if (get_bw_tl_br() < get_bw_bl_tr()) {
        from = rect.tl();
        to = rect.br();
    } else {
        from.x = rect.x;
        to.y = rect.y;
        from.y = rect.y + rect.height;
        to.x = rect.x + rect.width;
    }
}

void SOSO17Converter::LineBondItem::predFromToForWedge(const cv::Mat &_imgGray) {
    float w = rect.width, h = rect.height, x = rect.x, y = rect.y;
    int xmin = (std::max)(0, (int) std::floor(x));
    int ymin = (std::max)(0, (int) std::floor(y));
    int xmax = (std::min)(_imgGray.cols - 1, (int) std::ceil(x + w));
    int ymax = (std::min)(_imgGray.rows - 1, (int) std::ceil(y + h));
    int iw = xmax - xmin + 1, ih = ymax - ymin + 1;
    // 边长足够小 or 足够窄，误差忽略不计，直接取中点作为起始点
    if (iw < sMinSize || ih < sMinSize || w / h > sLineThresh || h / w > sLineThresh) {
        if (w < h) {
            from.x = to.x = x + w / 2;
            from.y = y;
            to.y = y + h;
            auto s1 = getScalarSum(cv::sum(_imgGray(cv::Rect2i(
                    xmin, ymin, iw, ih / 2))));
            auto s2 = getScalarSum(cv::sum(_imgGray(cv::Rect2i(
                    xmin, ymin + ih / 2, iw, ih / 2))));
            if (s1 < s2) {
                std::swap(from, to);
            }
        } else {
            from.x = x;
            to.x = x + w;
            from.y = to.y = y + h / 2;
            auto s1 = getScalarSum(cv::sum(_imgGray(cv::Rect2i(
                    xmin, ymin, iw / 2, ih))));
            auto s2 = getScalarSum(cv::sum(_imgGray(cv::Rect2i(
                    xmin + ih / 2, ymin, iw / 2, ih))));
            if (s1 < s2) {
                std::swap(from, to);
            }
        }
        return;
    }
    const int roiSize = 32;
    cv::Mat roiArea;
    cv::resize(_imgGray(cv::Rect2i(xmin, ymin, iw, ih)),
               roiArea, cv::Size(roiSize, roiSize));
    const int step = roiSize / 4;
    const int step2 = 2 * step;
    const int step3 = 3 * step;
    auto get_bw_tl = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(0, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step, step, step, step)))

                + cv::sum(roiArea(cv::Rect2i(0, 0, step2, step2))));
    };
    auto get_bw_br = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step2, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step3, step3, step, step)))

                + cv::sum(roiArea(cv::Rect2i(step2, step2, step2, step2))));
    };
    auto get_bw_bl = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step, step2, step, step)))
                + cv::sum(roiArea(cv::Rect2i(0, step3, step, step)))

                + cv::sum(roiArea(cv::Rect2i(0, step2, step2, step2))));
    };
    auto get_bw_tr = [&]() -> int {
        return getScalarSum(
                cv::sum(roiArea(cv::Rect2i(step3, 0, step, step)))
                + cv::sum(roiArea(cv::Rect2i(step2, step, step, step)))

                + cv::sum(roiArea(cv::Rect2i(step2, 0, step2, step2))));
    };
//    std::cout << get_bw_tl_br() << std::endl;
//    std::cout << get_bw_bl_tr() << std::endl;
//    cv::imshow("origin", _imgGray(cv::Rect2i(xmin, ymin, iw, ih)));
//    cv::imshow("roi64", roiArea);
//    cv::waitKey(0);
    auto s1 = get_bw_tl(), s2 = get_bw_tr(), s3 = get_bw_bl(), s4 = get_bw_br();
    if (s1 + s4 < s2 + s3) {
        from = rect.tl();
        to = rect.br();
        if (s1 < s4) {
            std::swap(from, to);
        }
    } else {
        from.x = rect.x;
        to.y = rect.y;
        from.y = rect.y + rect.height;
        to.x = rect.x + rect.width;
        if (s3 < s2) {
            std::swap(from, to);
        }
    }

//    std::cout<<s1<<","<<s2<<"\n"<<s3<<","<<s4<<"\n-------"<<std::endl;
//    cv::imshow("roi64", roiArea);
//    cv::Mat displayImg;
//    cv::cvtColor(_imgGray,displayImg,cv::COLOR_GRAY2BGR);
//    cv::rectangle(displayImg, rect, getScalar(ColorName::rgbGreen), 2);
//    cv::circle(displayImg, from, 4, getScalar(ColorName::rgbBlue), -1);
//    cv::circle(displayImg, to, 4, getScalar(ColorName::rgbRed), -1);
//    cv::imshow("bond", displayImg);
//    cv::waitKey(0);
}

[[deprecated("Replaced by SOSO17Converter::LineBondItem::predFromTo")]]
std::pair<cv::Point2f, cv::Point2f> extractBondFromTo(const cv::Mat &_img, const YoloObject &_obj) {
    // 是否方向无关
    bool isDirNeeded;
    switch (convertLabelToBondType(_obj.label)) {
        case JBondType::SolidWedgeBond:
        case JBondType::DashWedgeBond:
            isDirNeeded = true;
            break;
        default:
            isDirNeeded = false;
    }
    cv::Rect rect(_obj.x, _obj.y, _obj.w, _obj.h);
    // 是否扁平
    float k = rect.width / rect.height;
    std::cout << rect << std::endl;
    std::cout << "width/height=" << k << std::endl;
    const float kThresh = 3;
    const int paddingSize = 2;
    bool isVertical = k < 1.0 / kThresh;
    bool isHorizontal = k > kThresh;
    bool isLineLike = isVertical || isHorizontal;
    // RIO box
    cv::Rect2i rect2i = rect;
    // 越界处理
    rect2i.x = (std::max)(0, rect2i.x - paddingSize);
    rect2i.y = (std::max)(0, rect2i.y - paddingSize);
    rect2i.width = (std::min)(rect2i.x + _img.rows - 1, rect2i.width + 2 * paddingSize);
    rect2i.height = (std::min)(rect2i.y + _img.cols - 1, rect2i.height + 2 * paddingSize);
    // 扁平且方向无关
    cv::Point2f from, to;
    if (isLineLike) {
        // 返回短边中点
        if (isVertical) {
            from.x = to.x = rect.x + rect.width / 2;
            from.y = rect.y;
            to.y = rect.y + rect.height;
        } else {
            from.x = rect.x;
            to.x = rect.x + rect.width;
            from.y = to.y = rect.y + rect.height / 2;
        }
        // 在输入图片边长是32的倍数的前提下，此时rect2i必然是一个边长至少为4且在图像内的框
        if (isDirNeeded) {
            // 判断是否需要交换(from,to)
            // 策略：区域二分，计算像素均值
            cv::Scalar scalar1, scalar2;
            if (isVertical) {
                // 上下分割
                auto mask1 = _img(cv::Rect2i(rect2i.x, rect2i.y,
                                             rect2i.width, rect2i.height / 2));
                auto mask2 = _img(cv::Rect2i(rect2i.x, rect2i.y + rect2i.height / 2,
                                             rect2i.width, rect2i.height / 2));
                scalar1 = cv::mean(std::move(mask1));
                scalar2 = cv::mean(std::move(mask2));
            } else {
                // 左右分割
                auto mask1 = _img(cv::Rect2i(rect2i.x, rect2i.y,
                                             rect2i.width / 2, rect2i.height));
                auto mask2 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2, rect2i.y,
                                             rect2i.width / 2, rect2i.height));
                scalar1 = cv::mean(std::move(mask1));
                scalar2 = cv::mean(std::move(mask2));
            }
//            if (debug) {
//                std::cout << getScalarSum(scalar1) << ", " << getScalarSum(scalar2) << std::endl;
//                std::cout << "----------------\n";
//            }
            // 白色像素多 -> sum 大 && 设为 from
            bool isSwapNeeded = getScalarSum(scalar1) < getScalarSum(scalar2);
            if (isSwapNeeded) {
                std::swap(from, to);
            }
        }
    } else if (isDirNeeded) {
        auto mask = _img(rect2i);
        cv::cvtColor(mask, mask, cv::COLOR_BGR2GRAY);
        auto getWedgeTemplate = [&](
                const int &_w, const int &_h, const int &_direction, const float &_k = 2) {
            cv::Mat tmpMat(_h, _w, CV_8UC1,
                           getScalar(ColorName::rgbWhite));
            std::vector<cv::Point2i> pts(3);
            const float up = _k / (_k + 1);
            const float down = 1 - up;
            switch (_direction) {
                case 0:
                    pts[0] = {0, 0};
                    pts[1] = {_w - 1, (int) (_h * up)};
                    pts[2] = {(int) (_w * up), _h - 1};
                    break;
                case 1:
                    pts[0] = {_w - 1, 0};
                    pts[1] = {0, (int) (_h * up)};
                    pts[2] = {(int) (_w * down), _h - 1};
                    break;
                case 2:
                    pts[0] = {0, _h - 1};
                    pts[1] = {_w - 1, (int) (_h * down)};
                    pts[2] = {(int) (_w * up), 0};
                    break;
                case 3:
                    pts[0] = {_w - 1, _h - 1};
                    pts[1] = {0, (int) (_h * down)};
                    pts[2] = {(int) (_w * down), 0};
                    break;
            }
            cv::fillPoly(tmpMat, pts, getScalar(ColorName::rgbBlack), cv::LINE_AA);
//        cv::imshow("fuck",tmpMat);
//        cv::waitKey(0);
            return std::move(tmpMat);
        };
        auto getFuckCount = [&](const cv::Mat &_m1,
                                const cv::Mat &_m2) -> double {
            double fuck = 0;
            for (int i = 0; i < _m1.rows; i++) {
                for (int j = 0; j < _m1.cols; j++) {
                    if (_m2.at<uchar>(i, j) == 0) { // 黑
                        if (_m1.at<uchar>(i, j) == 0) {
                            fuck += 1;
                        } else {
                            fuck -= 1;
                        }
                    }
                }
            }
            return fuck;
        };
        double d0 = getFuckCount(mask, getWedgeTemplate(
                rect2i.width, rect2i.height, 0));
        double d1 = getFuckCount(mask, getWedgeTemplate(
                rect2i.width, rect2i.height, 1));
        double d2 = getFuckCount(mask, getWedgeTemplate(
                rect2i.width, rect2i.height, 2));
        double d3 = getFuckCount(mask, getWedgeTemplate(
                rect2i.width, rect2i.height, 3));
        cv::Point maxLoc;
        cv::minMaxLoc(std::vector<double>({d0, d1, d2, d3}),
                      nullptr, nullptr, nullptr, &maxLoc);
        from = to = rect.tl();
        switch (maxLoc.x) {
            case 0:
                to = rect.br();
                break;
            case 1:
                from.x += rect.width;
                to.y += rect.height;
                break;
            case 2:
                from.y += rect.height;
                to.x += rect.width;
                break;
            case 3:
                from = rect.br();
                break;
        }
        std::cout << d0 << ", " << d1 << std::endl;
        std::cout << d2 << ", " << d3 << std::endl;
        std::cout << "----------------\n";
    } else {
        // 非扁平矩形，区域四分(1234左上右上左下右下)，统计交叉网格的特征像素计数
        auto mask0 = _img(cv::Rect2i(rect2i.x, rect2i.y,
                                     rect2i.width / 2, rect2i.height / 2));
        auto mask1 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2, rect2i.y,
                                     rect2i.width / 2, rect2i.height / 2));
        auto mask2 = _img(cv::Rect2i(rect2i.x, rect2i.y + rect.height / 2,
                                     rect2i.width / 2, rect2i.height / 2));
        auto mask3 = _img(cv::Rect2i(rect2i.x + rect2i.width / 2,
                                     rect2i.y + rect2i.height / 2,
                                     rect2i.width / 2, rect2i.height / 2));
        auto sum0 = getScalarSum(cv::mean(std::move(mask0)));
        auto sum1 = getScalarSum(cv::mean(std::move(mask1)));
        auto sum2 = getScalarSum(cv::mean(std::move(mask2)));
        auto sum3 = getScalarSum(cv::mean(std::move(mask3)));
        if (sum0 + sum3 < sum1 + sum2) {
            from = rect.tl();
            to = rect.br();
        } else {
            from.x = rect.x;
            from.y = rect.y + rect.height;
            to.x = rect.x + rect.width;
            to.y = rect.y;
        }
        std::cout << sum0 << ", " << sum1 << std::endl;
        std::cout << sum2 << ", " << sum3 << std::endl;
        std::cout << "----------------\n";
    }
    auto tmpMat = _img.clone();
    cv::putText(tmpMat, "[0]", from, 1, 1.2,
                getScalar(ColorName::rgbOrangeRed), 2,
                cv::LINE_AA, false);
    cv::putText(tmpMat, "[1]", to, 1, 1.2,
                getScalar(ColorName::rgbDarkRed), 2,
                cv::LINE_AA, false);
    cv::imshow("BoxGraphConverter-getFromTo4LineBond", tmpMat);
    cv::waitKey(0);
    return {from, to};
}
