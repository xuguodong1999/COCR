#include "hw_mol.hpp"
#include "hw_data.hpp"
#include "mol2d.hpp"
#include "mol_op.hpp"

#include "timer.hpp"
#include "colors.hpp"

#include "opencv_util.hpp"
#include "std_util.hpp"
#include "crnn_data.hpp"

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
    auto mol2d = std::static_pointer_cast<Mol2D>(mol2dHolder);
    auto molOp = std::static_pointer_cast<MolOp>(molOpHolder);
    mol2d->calcCoord2D();
    float avgBondLength = mol2d->getAvgBondLength();
    mData.clear();
    // 记录显式写出的原子 id
    std::unordered_map<size_t, bool> explicitAtomMap;
    auto &dataLoader = HwDataLoader::getInstance();
    // 添加原子图元
    auto add_atom_item = [&](const size_t &_aid) {
        auto atom = mol->getAtomById(_aid);
        auto sym = std::make_shared<HwStr>(atom->getElementType());
        sym->setKeepDirection(true);
        float fontSize = avgBondLength * betweenProb(0.3, 0.7);
        sym->resizeTo(fontSize, fontSize);
        sym->moveCenterTo(mol2d->getPos2D(_aid));
        if (ElementType::C == atom->getElementType()) {
            explicitAtomMap[_aid] = byProb(_explicitCarbonProb);
        } else {
            explicitAtomMap[_aid] = true;
        }
        if (explicitAtomMap[_aid]) {
            mData.push_back(std::move(sym));
            hwToAtomMap[mData.size() - 1] = _aid;// 记录映射用于字符转字符串时的化合价评估
        }
    };
    mol->safeTraverseAtoms(add_atom_item);
    auto &bondInRing = molOp->getAromaticRings(false);

    std::vector<std::unordered_set<size_t>> kekuleRings;// 记录画圈圈的化学键id
    std::unordered_set<size_t> bondInKekuleRings;// 记录画圈圈的化学键id，用于快速查找
    for (auto &aromaticStruct:bondInRing) {
        if (byProb(0.8)) {
            for (auto &ring:aromaticStruct) {
                kekuleRings.push_back(ring);// 深拷贝
                for (auto &bid:ring) {
                    bondInKekuleRings.insert(bid);
                }
            }
        }
    }
    // 统计单折线
    //<键id，键id>
    std::unordered_map<size_t, size_t> lineBondMap;
    //<键id,图元下标>
    std::unordered_map<size_t, size_t> itemBondMap;
    // <原子id,这个原子关联的键>
    std::unordered_map<size_t, std::unordered_set<size_t>> atomBondMap;
    auto init_ab_map = [&](const size_t &_bid) {
        auto bond = mol->getBondById(_bid);
        atomBondMap[bond->getAtomFrom()].insert(_bid);
        atomBondMap[bond->getAtomTo()].insert(_bid);
    };
    mol->safeTraverseBonds(init_ab_map);
    for (auto&[aid, bidSet]:atomBondMap) {
        if (bidSet.size() != 2 || explicitAtomMap[aid])
            continue;
        std::vector<size_t> tmp;
        for (auto &bid:bidSet) {
            tmp.push_back(bid);
        }
        if (mol->getBondById(tmp[0])->getBondType() != JBondType::SingleBond)
            continue;
        if (mol->getBondById(tmp[1])->getBondType() != JBondType::SingleBond)
            continue;
        if (tmp[0] < tmp[1])std::swap(tmp[0], tmp[1]);
        lineBondMap.insert({tmp[0], tmp[1]});
    }
    atomBondMap.clear();
    // 添加键坐标
    auto add_bond_item = [&](const size_t &_bid) {
        auto bond = mol->getBondById(_bid);
        JBondType bondType = bond->getBondType();
        if (bondInKekuleRings.end() != bondInKekuleRings.find(_bid)) {
            bondType = JBondType::DelocalizedBond;  // 不能影响 JMol 里的数据
        }
        auto sym = HwBond::GetHwBond(bondType);
//        shared_ptr<BondItem> sym = BondItem::GetBond(bondType);
//        sym->setUseHandWrittenWChar(true);
        auto from_ = mol2d->getPos2D(bond->getAtomFrom());
        auto to_ = mol2d->getPos2D(bond->getAtomTo());
        cv::Point2f from = from_, to = to_;
        const float k = 0.3;
        if (explicitAtomMap[bond->getAtomFrom()]) {
            from = (1 - k) * (from_ - to_) + to_;
        }
        if (explicitAtomMap[bond->getAtomTo()]) {
            to = (1 - k) * (to_ - from_) + from_;
        }
        sym->setVertices({from, to});
        if (bondType == JBondType::SingleBond) {
            itemBondMap[_bid] = mData.size();
        }
        mData.push_back(std::move(sym));
    };
    mol->safeTraverseBonds(add_bond_item);
    // 加环
    for (auto &ring:kekuleRings) {
        auto sym = HwBond::GetHwBond(JBondType::CircleBond);
//        auto sym = BondItem::GetBond(JBondType::CircleBond);
        std::vector<cv::Point2f> pts;
        std::unordered_set<size_t> aids;
        for (auto &bid:ring) {
            auto bond = mol->getBondById(bid);
            aids.insert(bond->getAtomFrom());
            aids.insert(bond->getAtomTo());
        }
        for (auto &aid:aids) {
            pts.push_back(mol2d->getPos2D(aid));
        }
        sym->setVertices(pts);
        mData.push_back(std::move(sym));
    }
    float avgSize = 0;
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
        std::vector<size_t> aids = {mol->getBondById(bid1)->getAtomFrom(),
                                    mol->getBondById(bid1)->getAtomTo(),
                                    mol->getBondById(bid2)->getAtomFrom(),
                                    mol->getBondById(bid2)->getAtomTo()};
        for (auto &id:aids) {
            if (notExist(tmp, id)) {
                tmp.insert(id);
            } else {
                aid = id;
                break;
            }
        }
        // aid is angle atom
        cv::Point2f A = mol2d->getPos2D(aid);
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
    // 添加字符串操作在写入画布前的深拷贝对象上完成
//    std::cout << "avgSize=" << avgSize << std::endl;
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
        {DetectorClasses::ItemSingleBond, 0},
        {DetectorClasses::ItemDoubleBond, 1},
        {DetectorClasses::ItemTripleBond, 2},
        {DetectorClasses::ItemSolidWedgeBond, 3},
        {DetectorClasses::ItemDashWedgeBond, 4},
        {DetectorClasses::ItemWaveBond, 5},
        {DetectorClasses::ItemCircleBond, 6},
        // deprecated char type
        {DetectorClasses::ItemC, 7},
        {DetectorClasses::ItemH, 7},
        {DetectorClasses::ItemO, 7},
        {DetectorClasses::ItemN, 7},
        {DetectorClasses::ItemP, 7},
        {DetectorClasses::ItemS, 7},
        {DetectorClasses::ItemF, 7},
        {DetectorClasses::ItemCl, 7},
        {DetectorClasses::ItemBr, 7},
        {DetectorClasses::ItemI, 7},
        {DetectorClasses::ItemB, 7},
        // new text type
        {DetectorClasses::ItemHorizontalStr, 7}
};
static std::vector<HwController> controllers = {
//        HwController(2),
        HwController(3),
        HwController(4),
        HwController(5),
        HwController(6),
        HwController(7),
        HwController(8),
};

void HwMol::dumpAsDarknet(const std::string &_imgPath, const std::string &_labelPath,
                          const size_t &_repeatTimes) {
    float avgSize = reloadHWData(0.1);
    setHwController(controllers[rand() % controllers.size()]);
    float k = 100.0f / (std::max)(0.01f, avgSize);
    this->mulK(k, k);
    size_t fixW, fixH;
    fixW = fixH = 640;
    for (size_t i = 0; i < _repeatTimes; i++) {
        this->rotate(rand() % 360);
        auto bBox = this->getBoundingBox().value();
        const int minWidth = 8 + bBox.width, minHeight = 8 + bBox.height;
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        this->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        this->paintTo(img);
        std::string suffix = "_" + std::to_string(i);
        auto paddingColor = getScalar(ColorName::rgbWhite);
        if (byProb(hwController->getRevertColorProb())) {// 反转颜色
            cv::bitwise_not(img, img);
            paddingColor = getScalar(ColorName::rgbBlack);
        }
        auto[resImg, offset]=resizeCvMatTo(img, fixW, fixH, paddingColor);
        auto&[k, offsetx, offsety]=offset;
        cv::imwrite(_imgPath + suffix + ".jpg", resImg,
                    {cv::IMWRITE_JPEG_QUALITY, 100});
        std::ofstream ofsm(_labelPath + suffix + ".txt");
        ofsm.precision(6);
        if (!ofsm.is_open()) {
            std::cerr << "fail to open " << _labelPath + suffix + ".txt" << std::endl;
            exit(-1);
        }
        for (auto &sym:mData) {
            auto name = sym->getItemType();
            auto bBox = sym->getBoundingBox().value();
            bBox.x = bBox.x * k + offsetx;
            bBox.y = bBox.y * k + offsety;
            bBox.width *= k;
            bBox.height *= k;
            float centX = bBox.x + bBox.width / 2, centY = bBox.y + bBox.height / 2;
            ofsm << fullLabels[name] << " " << centX / fixW << " " << centY / fixH << " "
                 << bBox.width / fixW << " " << bBox.height / fixH << "\n";
        }
        ofsm.close();
    }
}

void HwMol::showOnScreen(const size_t &_repeatTimes, bool _showBox) {
    float avgSize = reloadHWData(0.1);
    float k0 = 100.0f / (std::max)(0.01f, avgSize);
    this->mulK(k0, k0);
    size_t fixW, fixH;
    fixW = fixH = 640;
    auto colorIdx = [](const int &_a) -> ColorName {
        return static_cast<const ColorName>((7 + _a) * 13 % 455);
    };
    for (size_t i = 0; i < _repeatTimes; i++) {
        auto target = std::dynamic_pointer_cast<HwMol>(this->clone());
        target->rotate(rand() % 360);
        target->replaceCharWithText(1);
        target->setHwController(controllers[rand() % controllers.size()]);
        auto bBox = target->getBoundingBox().value();
        const int minWidth = 8 + bBox.width, minHeight = 8 + bBox.height;
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        target->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        target->paintTo(img);
        auto[resImg, offset]=resizeCvMatTo(img, fixW, fixH);
        auto&[k, offsetx, offsety]=offset;
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
        cv::imshow("MolHwItem::showOnScreen", resImg);
        cv::waitKey(0);
    }
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


HwMol::HwMol(std::shared_ptr<MolHolder> _molOpHolder, std::shared_ptr<MolHolder> _mol2dHolder,
             HwController *_hwController)
        : MolHolder(_molOpHolder->getMol()), mol2dHolder(_mol2dHolder), molOpHolder(_molOpHolder),
          hwController(_hwController) {
    if (!hwController) {
        hwController = &(HwBase::baseController);
    }
    // TODO: 校验两个holder持有同一个JMol对象
    if (!_mol2dHolder) {
        mol2dHolder = std::make_shared<Mol2D>(mol);
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
extern CRNNDataGenerator crnnDataGenerator;

void HwMol::replaceCharWithText(const float &_prob) {
    auto molOp = std::static_pointer_cast<MolOp>(molOpHolder);
    molOp->updateAtomValenceMap();
    std::vector<cv::Rect2f> rects(mData.size());
    float avgSize = 0;
    for (size_t i = 0; i < mData.size(); i++) {
        auto &item = mData[i];
        rects[i] = item->getBoundingBox().value();
        avgSize += (std::max)(rects[i].width, rects[i].height);
    }
    avgSize /= mData.size();
    auto iou = [](const cv::Rect &_r1, const cv::Rect &_r2) -> float {
        return -1;
    };
    /**
     * 为第idx个边框水平向左右搜索空白空间，返回可利用的空间，如果空间小于预定尺寸，则返回空值
     */
    auto calc_free_space = [&](const size_t &_curIdx) -> std::optional<std::pair<cv::Rect2f, bool>> {
        for (size_t i = 0; i < mData.size(); i++) {
            if (i == _curIdx)continue;
            if (iou(rects[i], rects[_curIdx]) <= 0)continue;
        }
//        return std::nullopt;
        bool isLeft = false;
        cv::Rect2f rect = rects[_curIdx];
        rect.width *= 20;
        return std::make_pair(rect, isLeft);
    };
    auto fill_rect_with_text = [&](const cv::Rect &_freeRect, bool _isLeft, const size_t &_curIdx) -> void {
        auto aid = hwToAtomMap[_curIdx];
        auto atom = mol->getAtomById(aid);
        frac val = molOp->getValByAtomId(aid);
        std::cout << "element=" << atom->getElementName() << ",curVal=" << val << std::endl;
        auto newItem = crnnDataGenerator.getRectStr(_freeRect, val.intValue(), _isLeft);
        if (newItem) {
            std::cout << "!!!!! replace\n";
            mData[_curIdx] = newItem;
            rects[_curIdx] = newItem->getBoundingBox().value();
        }
    };
    std::cout << "*****************\n";
    for (size_t i = 0; i < mData.size(); i++) {
        if (!notExist(bondClassSet, mData[i]->getItemType()))continue;
        auto freeRectOpt = calc_free_space(i);
        if (!freeRectOpt)continue;
        auto &[freeRect, isLeft] = freeRectOpt.value();
        fill_rect_with_text(freeRect, isLeft, i);
    }
}

shared_ptr<HwBase> HwMol::clone() const {
    auto sister = std::make_shared<HwMol>(molOpHolder, mol2dHolder, hwController);
    sister->keepDirection = keepDirection;
    sister->mData.resize(mData.size());
    for (size_t i = 0; i < mData.size(); i++) {
        sister->mData[i] = mData[i]->clone();
    }
    sister->hwToAtomMap = hwToAtomMap;// TODO: 这个值不会被修改，可改引用
    return sister;
}
