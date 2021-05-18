#include "hw_mol.hpp"
#include "hw_data.hpp"

#include "timer.hpp"
#include "colors.hpp"

#include "opencv_util.hpp"
#include "std_util.hpp"

#include <opencv2/opencv.hpp>

#include <filesystem>
#include <fstream>
#include <iomanip>

using namespace std;


void HwMol::paintTo(cv::Mat &canvas) const {
    for (auto &sym:mData) {
        sym->paintTo(canvas);
    }
}

void HwMol::rotateBy(float angle, const cv::Point2f &cent) {
    // 骨架直接旋转，字符只做平移
    for (auto &sym:mData) {
        sym->rotateBy(angle, cent);
        if (sym->isDirectionKept()) {
            sym->rotate(-angle);
        }
    }
}


void HwMol::mulK(float kx, float ky) {
    for (auto &s:mData) {
        s->mulK(kx, ky);
    }
}

void HwMol::setHwController(HwController &_hwController) {
    hwController = &_hwController;
    for (auto &sym:mData) {
        sym->setHwController(*hwController);
    }
}

std::optional<cv::Rect2f> HwMol::getBoundingBox() const {
    if (mData.empty())return std::nullopt;
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &stroke:mData) {
        auto bbox = stroke->getBoundingBox();
        if (!bbox)continue;
        minx = std::min(minx, bbox->x);
        miny = std::min(miny, bbox->y);
        maxx = std::max(maxx, bbox->x + bbox->width);
        maxy = std::max(maxy, bbox->y + bbox->height);
    }
    if (minx > maxx)return std::nullopt;
    return cv::Rect2f(minx, miny, maxx - minx, maxy - miny);
}

float HwMol::reloadHWData(const float &_explicitCarbonProb) {
    mol->generate2D();
    float avgBondLength = mol->getAvgBondLength2D();

    mData.clear();
//    // 记录显式写出的原子 id
    std::unordered_map<size_t, bool> explicitAtomMap;
    auto &dataLoader = HwDataLoader::getInstance();
//    // 添加原子图元
    mol->loopAtomVec([&](xgd::JAtom &atom) {
        auto sym = std::make_shared<HwStr>(atom.getType());
        sym->setKeepDirection(true);
        float fontSize = avgBondLength * betweenProb(0.45, 0.5);
        sym->resizeTo(fontSize, fontSize);
        sym->moveCenterTo(cv::Point2f(atom.x, atom.y));
        if (ElementType::C == atom.getType()) {
            explicitAtomMap[atom.getId()] = byProb(_explicitCarbonProb);
        } else {
            explicitAtomMap[atom.getId()] = true;
        }
        if (explicitAtomMap[atom.getId()]) {
            mData.push_back(std::move(sym));
            hwToAtomMap[mData.size() - 1] = atom.getId();// 记录映射用于字符转字符串时的化合价评估
        }
    });

    // 统计单折线
    //<键id，键id>
    std::unordered_map<size_t, size_t> lineBondMap;
    //<键id,图元下标>
    std::unordered_map<size_t, size_t> itemBondMap;
    // <原子id,这个原子关联的键>
    std::unordered_map<size_t, std::unordered_set<size_t>> atomBondMap;
    mol->loopBondVec([&](xgd::JBond &bond) {
        atomBondMap[bond.getFrom()->getId()].insert(bond.getId());
        atomBondMap[bond.getTo()->getId()].insert(bond.getId());
    });
    for (auto&[aid, bidSet]:atomBondMap) {
        if (bidSet.size() != 2 || explicitAtomMap[aid])
            continue;
        std::vector<size_t> tmp;
        for (auto &bid:bidSet) {
            tmp.push_back(bid);
        }
        if (mol->getBond(tmp[0])->getType() != xgd::BondType::SingleBond)
            continue;
        if (mol->getBond(tmp[1])->getType() != xgd::BondType::SingleBond)
            continue;
        if (tmp[0] < tmp[1])std::swap(tmp[0], tmp[1]);
        lineBondMap.insert({tmp[0], tmp[1]});
    }
    atomBondMap.clear();
//    // 添加键坐标
    mol->loopBondVec([&](xgd::JBond &bond) {
        xgd::BondType bondType = bond.getType();
        auto sym = HwBond::GetHwBond(bondType);
        cv::Point2f from_ = {bond.getFrom()->x, bond.getFrom()->y}, to_ = {bond.getTo()->x, bond.getTo()->y};
        cv::Point2f from = from_, to = to_;
        const float k = 0.3;
        if (explicitAtomMap[bond.getFrom()->getId()]) {
            from = (1 - k) * (from_ - to_) + to_;
        }
        if (explicitAtomMap[bond.getTo()->getId()]) {
            to = (1 - k) * (to_ - from_) + from_;
        }
        sym->setVertices({from, to});
        if (bondType == xgd::BondType::SingleBond) {
            itemBondMap[bond.getId()] = mData.size();
        }
        mData.push_back(std::move(sym));
    });

    avgSize = 0;
    for (auto &sym:mData) {
        auto bBox = sym->getBoundingBox();
        if (!bBox)continue;
        avgSize += std::max(bBox->width, bBox->height);
    }
    if (!mData.empty()) avgSize /= mData.size();
//    return avgSize;
    // 添加弧线转折
//    std::cout << "lineBondMap.size()=" << lineBondMap.size() << std::endl;
//    std::cout << "avgBondLength=" << avgSize << std::endl;
    for (auto&[bid1, bid2]:lineBondMap) {
        auto item1 = std::static_pointer_cast<HwSingleBond>(mData[itemBondMap[bid1]]);
        auto item2 = std::static_pointer_cast<HwSingleBond>(mData[itemBondMap[bid2]]);
        size_t aid;
        std::unordered_set<size_t> tmp;
        std::vector<size_t> aids = {mol->getBond(bid1)->getFrom()->getId(),
                                    mol->getBond(bid1)->getTo()->getId(),
                                    mol->getBond(bid2)->getFrom()->getId(),
                                    mol->getBond(bid2)->getTo()->getId()};
        for (auto &id:aids) {
            if (notExist(tmp, id)) {
                tmp.insert(id);
            } else {
                aid = id;
                break;
            }
        }
        // aid is angle atom
        cv::Point2f A = {mol->getAtom(aid)->x, mol->getAtom(aid)->y};
//        std::cout << "A=" << A << std::endl;
//        std::cout << "bbox1=" << item1->getBoundingBox().value() << std::endl;
//        std::cout << "bbox2=" << item2->getBoundingBox().value() << std::endl;
        const float threshDis = avgBondLength / 4.0;
        // 1、排除影响范围里的所有点
        cv::Point2f nearestPt, B, C;
        float minDis;
        auto keep_condition = [&](const cv::Point2f &_pt) -> bool {
            const float curDis = getDistance2D(_pt, A);
            bool res = (curDis > threshDis);
            if (res && curDis < minDis) {
                minDis = curDis;
                nearestPt = _pt;
            }
            return res;
        };
        minDis = std::numeric_limits<float>::max();
        item1->keepIf(keep_condition);
        B = nearestPt;

        minDis = std::numeric_limits<float>::max();
        item2->keepIf(keep_condition);
        C = nearestPt;
        auto D = (B + C) / 2.0;
        auto h = A - D;
        // 2、添加圆弧: 斜边 from-to，垂足 D
        auto add_arc = [&](const cv::Point2f &_from, const cv::Point2f &_to) -> HwScript {
            HwStroke s;
            float sx = _from.x, sy = _from.y;
            float k = 10;
            float dx = (D.x - _from.x) / k, dy = (D.y - _from.y) / k;
            for (int i = 0; i <= k; i++) {
                s.push_back(cv::Point2f(sx, sy) + h * std::sin(M_PI_2 * i / k));
                sx += dx;
                sy += dy;
            }
            HwScript arc;
            arc.push_back(s);
            return std::move(arc);
        };
        item1->push_back(add_arc(B, A));
        item2->push_back(add_arc(C, A));
    }
    return avgSize;

}

// soso17 labels
static std::unordered_map<DetectorClasses, int> soso17Labels = {
        {DetectorClasses::ItemO,              1},
        {DetectorClasses::ItemI,              2},
        {DetectorClasses::ItemC,              6},
        {DetectorClasses::ItemF,              12},
        {DetectorClasses::ItemN,              5},
        {DetectorClasses::ItemH,              4},
        {DetectorClasses::ItemDashWedgeBond,  9},
        {DetectorClasses::ItemSingleBond,     8},
        {DetectorClasses::ItemDoubleBond,     11},
        {DetectorClasses::ItemTripleBond,     13},
        {DetectorClasses::ItemCircleBond,     16},
        {DetectorClasses::ItemS,              3},
        {DetectorClasses::ItemP,              15},
        {DetectorClasses::ItemBr,             0},
        {DetectorClasses::ItemB,              7},
        {DetectorClasses::ItemSolidWedgeBond, 10},
        {DetectorClasses::ItemCl,             14}};
//static int beginLabel = 0;
static std::unordered_map<DetectorClasses, int> fullLabels = {
        // bond type
        {DetectorClasses::ItemSingleBond,     0},
        {DetectorClasses::ItemDoubleBond,     1},
        {DetectorClasses::ItemTripleBond,     2},
        {DetectorClasses::ItemSolidWedgeBond, 3},
        {DetectorClasses::ItemDashWedgeBond,  4},
        {DetectorClasses::ItemWaveBond,       5},
        {DetectorClasses::ItemCircleBond,     6},
        // deprecated char type
        {DetectorClasses::ItemC,              7},
        {DetectorClasses::ItemH,              7},
        {DetectorClasses::ItemO,              7},
        {DetectorClasses::ItemN,              7},
        {DetectorClasses::ItemP,              7},
        {DetectorClasses::ItemS,              7},
        {DetectorClasses::ItemF,              7},
        {DetectorClasses::ItemCl,             7},
        {DetectorClasses::ItemBr,             7},
        {DetectorClasses::ItemI,              7},
        {DetectorClasses::ItemB,              7},
        // new text type
        {DetectorClasses::ItemHorizontalStr,  7}
};


static std::vector<HwController> crude = {
        HwController(2),
        HwController(3),
};

std::shared_ptr<HwBase> HwMol::clone() const {
    return nullptr;
}

std::vector<cv::Mat> HwMol::showOnScreen(const size_t &_repeatTimes, bool _showBox) {
    reloadHWData(0.1);
    float k0 = betweenProb(75, 85) / (std::max)(0.01f, avgSize);
    this->mulK(k0, k0);
//    size_t fixW, fixH;
//    fixW = fixH = 640;
    auto colorIdx = [](const int &_a) -> ColorName {
        return static_cast<const ColorName>((7 + _a) * 13 % 455);
    };
    std::vector<cv::Mat> retImg;
    for (size_t i = 0; i < _repeatTimes; i++) {
        auto target = this;
        target->rotate(randInt() % 360);
        target->setHwController(crude[randInt() % crude.size()]);
        auto bBox = target->getBoundingBox().value();
        int minWidth = 64 + bBox.width, minHeight = 64 + bBox.height;
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        target->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        target->paintTo(img);

        auto[resImg0, offset]=resizeCvMatTo(img, minWidth, minHeight);
        retImg.push_back(resImg0);
        cv::Mat resImg = resImg0.clone();
        auto&[k, offsetx, offsety]=offset;
        int ow = bBox.width, oh = bBox.height;
        ow = ow * k;
        oh = oh * k;
        for (auto &sym:target->mData) {
//            std::cout << fullLabels[sym->getItemType()] << std::endl;
            auto bBox = sym->getBoundingBox().value();
            bBox.x = bBox.x * k + offsetx;
            bBox.y = bBox.y * k + offsety;
            bBox.width *= k;
            bBox.height *= k;
            if (_showBox)
                cv::rectangle(resImg, bBox, getScalar(
                        colorIdx((int) sym->getItemType())), 1, cv::LINE_AA);
        }
//        cv::imshow("MolHwItem::showOnScreen", resImg);
    }
    return retImg;
}

void HwMol::moveBy(const cv::Point2f &_offset) {
    for (auto &sym:mData) {
        sym->moveBy(_offset);
    }
}

void HwMol::rotate(float _angle) {
    auto center = getCenter();
    if (!center)return;
    rotateBy(_angle, center.value());
}


HwMol::HwMol(std::shared_ptr<xgd::JMol> _mol, HwController *_hwController)
        : mol(std::move(_mol)), hwController(_hwController), avgSize(-1) {
    if (!hwController) {
        hwController = &(HwBase::baseController);
    }
}


static std::unordered_set<DetectorClasses> bondClassSet = {
        DetectorClasses::ItemSingleBond,
        DetectorClasses::ItemDoubleBond,
        DetectorClasses::ItemTripleBond,
        DetectorClasses::ItemSolidWedgeBond,
        DetectorClasses::ItemDashWedgeBond,
        DetectorClasses::ItemCircleBond,
        DetectorClasses::ItemWaveBond,
};