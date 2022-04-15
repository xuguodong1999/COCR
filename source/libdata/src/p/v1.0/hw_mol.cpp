#include "hw_mol.h"


#include <filesystem>
#include <fstream>

using namespace std;

CRNNDataGenerator crnnDataGenerator;

void HwMol::paintTo(cv::Mat &canvas) const {
    for (auto &sym: mData) {
        sym->paintTo(canvas);
    }
}

void HwMol::rotateBy(float angle, const cv::Point2f &cent) {
    // 骨架直接旋转，字符只做平移
    for (auto &sym: mData) {
        sym->rotateBy(angle, cent);
        if (sym->isDirectionKept()) {
            sym->rotate(-angle);
        }
    }
}


void HwMol::mulK(float kx, float ky) {
    for (auto &s: mData) {
        s->mulK(kx, ky);
    }
}

void HwMol::setHwController(HwController &_hwController) {
    hwController = &_hwController;
    for (auto &sym: mData) {
        sym->setHwController(*hwController);
    }
}

std::optional<cv::Rect2f> HwMol::getBoundingBox() const {
    if (mData.empty())return std::nullopt;
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::lowest();
    for (auto &stroke: mData) {
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
    auto molOp = std::static_pointer_cast<MolOp>(molOpHolder);
    if (avgSize > 0) {
        molOp->updateAtomValenceMap();
        return avgSize;
    }
    auto mol2d = std::static_pointer_cast<Mol2D>(mol2dHolder);
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
        float fontSize = avgBondLength * betweenProb(0.2, 0.6);
        sym->resizeTo(fontSize, fontSize);
        const auto&[x, y]=mol2d->getPos2D(_aid);
        sym->moveCenterTo({x, y});
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
    for (auto &aromaticStruct: bondInRing) {
        if (byProb(0.8)) {
            for (auto &ring: aromaticStruct) {
                kekuleRings.push_back(ring);// 深拷贝
                for (auto &bid: ring) {
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
    for (auto&[aid, bidSet]: atomBondMap) {
        if (bidSet.size() != 2 || explicitAtomMap[aid])
            continue;
        std::vector<size_t> tmp;
        for (auto &bid: bidSet) {
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
        const auto&[fromX, fromY] = mol2d->getPos2D(bond->getAtomFrom());
        const auto&[toX, toY] = mol2d->getPos2D(bond->getAtomTo());
        cv::Point2f from, to, from_, to_;
        from = from_ = {fromX, fromY};
        to = to_ = {toX, toY};
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
    for (auto &ring: kekuleRings) {
        auto sym = HwBond::GetHwBond(JBondType::CircleBond);
//        auto sym = BondItem::GetBond(JBondType::CircleBond);
        std::vector<cv::Point2f> pts;
        std::unordered_set<size_t> aids;
        for (auto &bid: ring) {
            auto bond = mol->getBondById(bid);
            aids.insert(bond->getAtomFrom());
            aids.insert(bond->getAtomTo());
        }
        for (auto &aid: aids) {
            const auto&[x, y]=mol2d->getPos2D(aid);
            pts.push_back({x, y});
        }
        sym->setVertices(pts);
        mData.push_back(std::move(sym));
    }
    avgSize = 0;
    for (auto &sym: mData) {
        auto bBox = sym->getBoundingBox();
        if (!bBox)continue;
        avgSize += std::max(bBox->width, bBox->height);
    }
    if (!mData.empty()) avgSize /= mData.size();
//    return avgSize;
    // 添加弧线转折
//    std::cout << "lineBondMap.size()=" << lineBondMap.size() << std::endl;
//    std::cout << "avgBondLength=" << avgSize << std::endl;
    for (auto&[bid1, bid2]: lineBondMap) {
        auto item1 = std::static_pointer_cast<HwSingleBond>(mData[itemBondMap[bid1]]);
        auto item2 = std::static_pointer_cast<HwSingleBond>(mData[itemBondMap[bid2]]);
        size_t aid;
        std::unordered_set<size_t> tmp;
        std::vector<size_t> aids = {mol->getBondById(bid1)->getAtomFrom(),
                                    mol->getBondById(bid1)->getAtomTo(),
                                    mol->getBondById(bid2)->getAtomFrom(),
                                    mol->getBondById(bid2)->getAtomTo()};
        for (auto &id: aids) {
            if (notExist(tmp, id)) {
                tmp.insert(id);
            } else {
                aid = id;
                break;
            }
        }
        // aid is angle atom
        const auto&[x, y]=mol2d->getPos2D(aid);
        const cv::Point2f A = {x, y};
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
    molOp->updateAtomValenceMap();
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
static std::vector<HwController> thin = {
        HwController(1),
        HwController(2),
        HwController(3),
        HwController(4),
};
static std::vector<HwController> crude = {
        HwController(3),
        HwController(4),
        HwController(5),
        HwController(6),
        HwController(7),
        HwController(8),
};

void HwMol::dumpAsDarknet(const std::string &_imgPath, const std::string &_labelPath,
                          const size_t &_repeatTimes) {
    reloadHWData(0.1);
    setHwController(thin[randInt() % thin.size()]);
    float k = 100.0f / (std::max)(0.01f, avgSize);
    this->mulK(k, k);
    size_t fixW, fixH;
    fixW = fixH = 640;
    for (size_t i = 0; i < _repeatTimes; i++) {
        auto target = std::dynamic_pointer_cast<HwMol>(this->clone());
        target->rotate(randInt() % 360);
        target->replaceCharWithText(0.25);
        if (target->getMol()->bondsNum() <= 6) {
            target->setHwController(thin[randInt() % thin.size()]);
        } else {
            target->setHwController(crude[randInt() % thin.size()]);
        }
        auto bBox = target->getBoundingBox().value();
        int minWidth = 8 + bBox.width, minHeight = 8 + bBox.height;
        if (target->getMol()->bondsNum() <= 6) {
            float scale = betweenProb(1.5, 3);
            minHeight *= scale;
            minWidth *= scale;
        }
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        target->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        target->paintTo(img);
        auto paddingColor = getScalar(ColorName::rgbWhite);
        auto[resImg0, offset]=resizeCvMatTo(img, fixW, fixH, paddingColor);
        cv::Mat resImg = resImg0;
        auto&[k, offsetx, offsety]=offset;
        int ow = bBox.width, oh = bBox.height;
        ow = ow * k;
        oh = oh * k;
        std::vector<cv::Rect2f> extraBoxes;
        auto fill_rect = [&](const cv::Rect2i &_rect) -> void {
            auto textImg = crnnDataGenerator.getStandardLongText();
            if (textImg.empty())return;
            int tw = textImg.cols, th = textImg.rows;
            int mw = _rect.width - 10, mh = _rect.height - 10;
            if (tw < mw && th < mh) {
                int tx = 5 + randInt() % (mw - tw), ty = 5 + randInt() % (mh - th);
                tx += _rect.x;
                ty += _rect.y;
                cv::Rect2i roi(tx, ty, tw, th);
                textImg.copyTo(resImg(roi));
                extraBoxes.emplace_back(tx, ty, tw, th);
            }
        };
        if (ow > oh) {
            int deltaH = (fixH - oh) / 2;
            cv::Rect2i free1(0, 0, fixW, deltaH), free2(0, fixH - deltaH, fixW, deltaH);
            fill_rect(free1);
            fill_rect(free2);
        }

        std::string suffix = "_" + std::to_string(i);
        if (byProb(hwController->getRevertColorProb())) {// 反转颜色
            cv::bitwise_not(resImg, resImg);
        }
        cv::cvtColor(resImg, resImg, cv::COLOR_BGR2GRAY);
        cv::imwrite(_imgPath + suffix + ".jpg", resImg,
                    {cv::IMWRITE_JPEG_QUALITY, 60 + randInt() % 40});
        std::ofstream ofsm(_labelPath + suffix + ".txt");
        ofsm.precision(6);
        if (!ofsm.is_open()) {
            std::cerr << "fail to open " << _labelPath + suffix + ".txt" << std::endl;
            exit(-1);
        }
        for (auto &sym: target->mData) {
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
        for (auto &bBox: extraBoxes) {
            float centX = bBox.x + bBox.width / 2, centY = bBox.y + bBox.height / 2;
            ofsm << fullLabels[DetectorClasses::ItemHorizontalStr] << " " << centX / fixW << " " << centY / fixH << " "
                 << bBox.width / fixW << " " << bBox.height / fixH << "\n";
        }
        ofsm.close();
    }
}


void HwMol::showOnScreen(const size_t &_repeatTimes, bool _showBox) {
    reloadHWData(0.1);
    float k0 = 100.0f / (std::max)(0.01f, avgSize);
    this->mulK(k0, k0);
    size_t fixW, fixH;
    fixW = fixH = 640;
    auto colorIdx = [](const int &_a) -> ColorName {
        return static_cast<const ColorName>((7 + _a) * 13 % 455);
    };
    for (size_t i = 0; i < _repeatTimes; i++) {
        auto target = std::dynamic_pointer_cast<HwMol>(this->clone());
        target->rotate(randInt() % 360);
        target->replaceCharWithText(0.25);
        if (target->getMol()->bondsNum() <= 6) {
            target->setHwController(thin[randInt() % thin.size()]);
        } else {
            target->setHwController(crude[randInt() % thin.size()]);
        }
        auto bBox = target->getBoundingBox().value();
        int minWidth = 8 + bBox.width, minHeight = 8 + bBox.height;
        if (target->getMol()->bondsNum() <= 6) {
            float scale = betweenProb(1.5, 3);
            minHeight *= scale;
            minWidth *= scale;
        }
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        target->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        target->paintTo(img);

        auto[resImg0, offset]=resizeCvMatTo(img, fixW, fixH);
        cv::Mat resImg = resImg0;
        auto&[k, offsetx, offsety]=offset;
        int ow = bBox.width, oh = bBox.height;
        ow = ow * k;
        oh = oh * k;
        std::vector<cv::Rect2f> extraBoxes;
        auto fill_rect = [&](const cv::Rect2i &_rect) -> void {
            auto textImg = crnnDataGenerator.getStandardLongText();
            if (textImg.empty())return;
            int tw = textImg.cols, th = textImg.rows;
            int mw = _rect.width - 10, mh = _rect.height - 10;
            if (tw < mw && th < mh) {
                int tx = 5 + randInt() % (mw - tw), ty = 5 + randInt() % (mh - th);
                tx += _rect.x;
                ty += _rect.y;
                cv::Rect2i roi(tx, ty, tw, th);
                textImg.copyTo(resImg(roi));
                extraBoxes.emplace_back(tx, ty, tw, th);
            }
        };
        if (ow > oh) {
            int deltaH = (fixH - oh) / 2;
            cv::Rect2i free1(0, 0, fixW, deltaH), free2(0, fixH - deltaH, fixW, deltaH);
            fill_rect(free1);
            fill_rect(free2);
        }
        for (auto &sym: target->mData) {
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
        for (auto &bBox: extraBoxes) {
            if (_showBox)
                cv::rectangle(resImg, bBox, getScalar(
                        colorIdx((int) DetectorClasses::ItemHorizontalStr)), 1, cv::LINE_AA);
        }
        cv::imshow("MolHwItem::showOnScreen", resImg);
        cv::waitKey(0);
    }
}

void HwMol::moveBy(const cv::Point2f &_offset) {
    for (auto &sym: mData) {
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
          hwController(_hwController), avgSize(-1) {
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

void HwMol::replaceCharWithText(const float &_prob) {
    auto molOp = std::static_pointer_cast<MolOp>(molOpHolder);
//    molOp->updateAtomValenceMap();
    std::vector<cv::Rect2f> rects(mData.size());
    float avgSize = 0;
    for (size_t i = 0; i < mData.size(); i++) {
        auto &item = mData[i];
        rects[i] = item->getBoundingBox().value();
        avgSize += (std::max)(rects[i].width, rects[i].height);
    }
    avgSize /= mData.size();
    /**
     * 为第idx个边框水平向左右搜索空白空间，返回可利用的空间，如果空间小于预定尺寸，则返回空值
     */
    auto calc_free_space = [&](const size_t &_curIdx) -> std::optional<std::pair<cv::Rect2f, bool>> {
        /**
         * 规则：
         * 1、新空间不小于原空间
         * 2、若左右都有空间，随机返回方向，裁剪另一侧空间
         * 3、
         */
        auto &curRect = rects[_curIdx];
        float xBegin = -10000, xMid = curRect.x + curRect.width / 2,
                xEnd = 10000;
        float yBegin = curRect.y, yEnd = curRect.y + curRect.height;
        for (size_t i = 0; i < mData.size(); i++) {
            if (i == _curIdx)continue;
            auto &cmpRect = rects[i];
            float yBegin2 = cmpRect.y, yEnd2 = cmpRect.y + cmpRect.height;
            if (yBegin2 > yEnd || yEnd2 < yBegin)continue;// 不在水平线上
            float xBegin2 = cmpRect.x, xEnd2 = cmpRect.x + cmpRect.width;
            if (xEnd2 < xMid) {
                xBegin = (std::max)(xBegin, xEnd2);
            }
            if (xBegin2 > xMid) {
                xEnd = (std::min)(xEnd, xBegin2);
            }
            if (xBegin > xMid && xEnd < xMid)return std::nullopt;
        }
//        std::cout << xBegin << "," << xMid << "," << xEnd << std::endl;
        if (xBegin < xMid || xMid < xEnd) {
            bool isLeft = (curRect.width + curRect.x) < xEnd;
            if (xBegin == -10000)isLeft = false;
            cv::Rect2f rect = curRect;
            if (isLeft) {
                rect.width = xEnd - curRect.x;
            } else {
                rect.x = xBegin;
                rect.width = curRect.x + curRect.width - xBegin;
            }
            if (rect.width / curRect.width < 2)return std::nullopt;
            return std::make_pair(rect, isLeft);
        } else {
            return std::nullopt;
        }
    };
    auto fill_rect_with_text = [&](const cv::Rect &_freeRect, bool _isLeft, const size_t &_curIdx) -> void {
        auto aid = hwToAtomMap[_curIdx];
        auto atom = mol->getAtomById(aid);
        frac val = molOp->getValByAtomId(aid);
//        std::cout << "element=" << atom->getElementName() << ",curVal=" << val << std::endl;
        auto newItem = crnnDataGenerator.getRectStr(_freeRect, val.intValue(), _isLeft);
        if (newItem) {
//            std::cout << "!!!!! replace\n";
            mData[_curIdx] = newItem;
            rects[_curIdx] = newItem->getBoundingBox().value();
        }
    };
//    std::cout << "*****************\n";
    for (size_t i = 0; i < mData.size(); i++) {
        if (!notExist(bondClassSet, mData[i]->getItemType()))continue;
        if (!byProb(_prob))continue;
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

std::shared_ptr<HwMol> HwMol::GetSpecialExample(float _explicitCarbonProb) {
    auto mol = std::make_shared<JMol>();
    auto molOp = std::make_shared<MolOp>(mol);
    auto example = std::make_shared<HwMol>(molOp);
    std::vector<size_t> atomicNumbers;
    std::vector<std::shared_ptr<JAtom>> newAtoms;
    std::vector<std::shared_ptr<JBond>> newBonds;
    auto add_c6 = [&]() {
        atomicNumbers = {5, 6, 7, 8, 15, 16};
        newAtoms = {
                mol->addAtom(6), mol->addAtom(6),
                mol->addAtom(6), mol->addAtom(6),
                mol->addAtom(6), mol->addAtom(randSelect(atomicNumbers))
        };
        std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
        newBonds = {
                mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                mol->addBond(newAtoms[1]->getId(), newAtoms[2]->getId()),
                mol->addBond(newAtoms[2]->getId(), newAtoms[3]->getId()),
                mol->addBond(newAtoms[3]->getId(), newAtoms[4]->getId()),
                mol->addBond(newAtoms[4]->getId(), newAtoms[5]->getId()),
                mol->addBond(newAtoms[5]->getId(), newAtoms[0]->getId()),
        };
        static std::vector<std::vector<std::pair<float, float>>> c6Coords2d = {
                {{78,  324}, {132, 224}, {265, 268}, {352, 237}, {292, 339}, {172, 291}},
                {{433, 231}, {522, 260}, {643, 221}, {699, 318}, {615, 286}, {483, 328}},
                {{475, 317}, {522, 371}, {583, 371}, {629, 321}, {608, 415}, {500, 413}}
        };
        const auto &coord_template = randSelect(c6Coords2d);
        for (size_t i = 0; i < 6; i++) {
            const auto&[x, y]=coord_template[i];
            newAtoms[i]->setCoord2d(x, y);
        }
    };
    auto add_grid2 = [&]() {
        atomicNumbers = {5, 6, 6, 6, 6, 6, 6, 6, 7, 8, 15, 16};
        std::vector<size_t> beginAtomicNumbers = {1, 3, 11, 19, 9, 17, 35, 53, 5, 6, 6, 6, 6, 6, 6, 6, 6, 7, 8, 15, 16};
        size_t loop = randInt() % 5 + 2;
        cv::Point2f pos1(11, 10), pos2(10, 11), offset1(0, 1), offset2(1, 0);
        auto last1 = mol->addAtom(randSelect(beginAtomicNumbers), pos1.x, pos1.y);
        auto last2 = mol->addAtom(randSelect(beginAtomicNumbers), pos2.x, pos2.y);;
        for (size_t i = 0; i < loop; i++) {
            pos1 += offset1;
            auto ab = mol->addAtom(6, pos1.x, pos1.y);
            JBondType bt1, bt2;
            bt1 = bt2 = JBondType::SingleBond;
            if (byProb(0.35)) {
                bt1 = JBondType::SolidWedgeBond;
                bt2 = JBondType::DashWedgeBond;
            } else if (byProb(0.25)) {
                bt1 = JBondType::WaveBond;
            }
            if (byProb(0.5)) {
                std::swap(bt1, bt2);
            }
            mol->addBond(ab->getId(), last1->getId(), bt1);
            mol->addBond(ab->getId(), last2->getId(), bt2);
            pos1 += offset1;
            auto a = mol->addAtom(randSelect(atomicNumbers), pos1.x, pos1.y);
            pos2 += 2 * offset2;
            auto b = mol->addAtom(randSelect(atomicNumbers), pos2.x, pos2.y);
            mol->addBond(a->getId(), ab->getId());
            mol->addBond(b->getId(), ab->getId());
            std::swap(offset1, offset2);
            last1 = a;
            last2 = b;
        }
        if (byProb(0.5)) {
            pos1 += offset1;
            auto ab = mol->addAtom(randSelect(atomicNumbers), pos1.x, pos1.y);
            mol->addBond(last1->getId(), ab->getId());
            mol->addBond(last2->getId(), ab->getId());
        }
    };
    auto add_tetrahedral = [&]() {
        atomicNumbers = {5, 6, 7, 8, 15, 16};
        newAtoms = {
                mol->addAtom(6), mol->addAtom(6),
                mol->addAtom(6), mol->addAtom(6),
                mol->addAtom(randSelect(atomicNumbers))
        };
        std::shuffle(newAtoms.begin(), newAtoms.end(), std::default_random_engine());
        newBonds = {
                mol->addBond(newAtoms[0]->getId(), newAtoms[1]->getId()),
                mol->addBond(newAtoms[0]->getId(), newAtoms[2]->getId()),
                mol->addBond(newAtoms[0]->getId(), newAtoms[3]->getId()),
                mol->addBond(newAtoms[0]->getId(), newAtoms[4]->getId()),
        };
        static std::vector<std::vector<std::pair<float, float>>> c5Coords2d = {
                {{348, 328}, {348, 265}, {299, 356}, {387, 368}, {408, 341}}
        };
        const auto &coord_template = randSelect(c5Coords2d);
        for (size_t i = 0; i < 5; i++) {
            const auto&[x, y]=coord_template[i];
            newAtoms[i]->setCoord2d(x, y);
        }
    };
    std::vector<std::function<void(void)>> func_list = {add_c6, add_tetrahedral, add_grid2};
    randSelect(func_list)();
    auto &mData = example->mData;
    auto &hwToAtomMap = example->hwToAtomMap;
    auto &avgSize = example->avgSize;
    mData.clear();
    // 记录显式写出的原子 id
    std::unordered_map<size_t, bool> explicitAtomMap;
    auto &dataLoader = HwDataLoader::getInstance();
    avgSize = 0;
    auto calc_avg_bond_lenth = [&](const size_t &_bid) -> void {
        auto bond = mol->getBondById(_bid);
        auto atomFrom = mol->getAtomById(bond->getAtomFrom());
        cv::Point2f from_(atomFrom->x, atomFrom->y);
        auto atomTo = mol->getAtomById(bond->getAtomTo());
        cv::Point2f to_(atomTo->x, atomTo->y);
        avgSize += getDistance2D(from_, to_);
    };
    mol->safeTraverseBonds(calc_avg_bond_lenth);
    avgSize /= mol->bondsNum();
    // 添加原子图元
    auto add_atom_item = [&](const size_t &_aid) {
        auto atom = mol->getAtomById(_aid);
        auto sym = std::make_shared<HwStr>(atom->getElementType());
        sym->setKeepDirection(true);
        float fontSize = avgSize * betweenProb(0.2, 0.6);
        sym->resizeTo(fontSize, fontSize);
        sym->moveCenterTo(cv::Point2f(atom->x, atom->y));
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
    for (auto&[aid, bidSet]: atomBondMap) {
        if (bidSet.size() != 2 || explicitAtomMap[aid])
            continue;
        std::vector<size_t> tmp;
        for (auto &bid: bidSet) {
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
        auto sym = HwBond::GetHwBond(bondType);
//        shared_ptr<BondItem> sym = BondItem::GetBond(bondType);
//        sym->setUseHandWrittenWChar(true);
        auto atomFrom = mol->getAtomById(bond->getAtomFrom());
        auto from_ = cv::Point2f(atomFrom->x, atomFrom->y);
        auto atomTo = mol->getAtomById(bond->getAtomTo());
        auto to_ = cv::Point2f(atomTo->x, atomTo->y);
//        std::cout << "from=" << from_ << ",to=" << to_ << std::endl;
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
    for (auto&[bid1, bid2]: lineBondMap) {
        auto item1 = std::static_pointer_cast<HwSingleBond>(mData[itemBondMap[bid1]]);
        auto item2 = std::static_pointer_cast<HwSingleBond>(mData[itemBondMap[bid2]]);
        size_t aid;
        std::unordered_set<size_t> tmp;
        std::vector<size_t> aids = {mol->getBondById(bid1)->getAtomFrom(),
                                    mol->getBondById(bid1)->getAtomTo(),
                                    mol->getBondById(bid2)->getAtomFrom(),
                                    mol->getBondById(bid2)->getAtomTo()};
        for (auto &id: aids) {
            if (notExist(tmp, id)) {
                tmp.insert(id);
            } else {
                aid = id;
                break;
            }
        }
        // aid is angle atom
        auto curAtom = mol->getAtomById(aid);
        cv::Point2f A(curAtom->x, curAtom->y);
//        std::cout << "A=" << A << std::endl;
//        std::cout << "bbox1=" << item1->getBoundingBox().value() << std::endl;
//        std::cout << "bbox2=" << item2->getBoundingBox().value() << std::endl;
        const float threshDis = avgSize / 4.0;
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
    molOp->updateAtomValenceMap();
    return example;
}

void HwMol::showSpecialExample(const size_t &_repeatTimes, bool _showBox) {
//    std::cout << "avgSize=" << avgSize << std::endl;
    float k0 = 100.0f / (std::max)(0.01f, avgSize);
    this->mulK(k0, k0);
    size_t fixW, fixH;
    fixW = fixH = 640;
    auto colorIdx = [](const int &_a) -> ColorName {
        return static_cast<const ColorName>((7 + _a) * 13 % 455);
    };
    for (size_t i = 0; i < _repeatTimes; i++) {
        auto target = std::dynamic_pointer_cast<HwMol>(this->clone());
        target->rotate(randInt() % 360);
//        target->replaceCharWithText(1);
        target->setHwController(thin[randInt() % thin.size()]);
        auto bBox = target->getBoundingBox().value();
        int minWidth = 8 + bBox.width, minHeight = 8 + bBox.height;
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        target->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        target->paintTo(img);

        auto[resImg, offset]=resizeCvMatTo(img, fixW, fixH);
        auto&[k, offsetx, offsety]=offset;
        for (auto &sym: target->mData) {
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
