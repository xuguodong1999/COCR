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

HwMol::HwMol(const JMol &_jmol) : mol(_jmol) {
    hwController = &(HwBase::baseController);
}

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
        sym->setHwController(_hwController);
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

float HwMol::reloadHWData(const float &_showCProb) {
    mData.clear();
    // 记录显式写出的原子 id
    std::unordered_map<size_t, bool> explicitAtomMap;
    auto atomPosMap = mol.get2DCoordinates();
    auto &dataLoader = HwDataLoader::getInstance();
    for (auto&[id, atom]:mol.getAtomsMap()) {
        auto sym = std::make_shared<HwStr>(atom->getElementName());
        sym->setKeepDirection(true);
        float fontSize = mol.getFontSize() * betweenProb(0.3, 0.7);
        sym->resizeTo(fontSize, fontSize);
        sym->moveCenterTo(atomPosMap[id]);
        if (ElementType::C == atom->getElementType()) {
            explicitAtomMap[id] = byProb(_showCProb);
        } else {
            explicitAtomMap[id] = true;
        }
        if (explicitAtomMap[id]) {
            mData.push_back(std::move(sym));
        }
    }
    auto &bondInRing = mol.getAromaticRings(false);
    std::vector<std::unordered_set<size_t>> kekuleRings;// 记录画圈圈的化学键id
    std::unordered_set<size_t> bondInKekuleRings;// 记录画圈圈的化学键id，用于快速查找
    for (auto &aromaticStruct:bondInRing) {
        if (byProb(0.5)) {
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
    for (auto&[id, bond]:mol.getBondsMap()) {
        atomBondMap[bond->getAtomFrom()].insert(id);
        atomBondMap[bond->getAtomTo()].insert(id);
    }
    for (auto&[aid, bidSet]:atomBondMap) {
        if (bidSet.size() != 2 || explicitAtomMap[aid])continue;
        std::vector<size_t> tmp;
        for (auto &bid:bidSet) {
            tmp.push_back(bid);
        }
        if (mol.getBondsMap().find(tmp[0])->second->getBondType() != JBondType::SingleBond)
            continue;
        if (mol.getBondsMap().find(tmp[1])->second->getBondType() != JBondType::SingleBond)
            continue;
        if (tmp[0] < tmp[1])std::swap(tmp[0], tmp[1]);
        lineBondMap.insert({tmp[0], tmp[1]});
    }
    atomBondMap.clear();
    // 添加键坐标
    for (auto&[bid, bond]:mol.getBondsMap()) {
        JBondType bondType = bond->getBondType();
        if (bondInKekuleRings.end() != bondInKekuleRings.find(bid)) {
            bondType = JBondType::DelocalizedBond;  // 不能影响 JMol 里的数据
        }
        auto sym = HwBond::GetHwBond(bondType);
//        shared_ptr<BondItem> sym = BondItem::GetBond(bondType);
//        sym->setUseHandWrittenWChar(true);
        auto from_ = atomPosMap[bond->getAtomFrom()];
        auto to_ = atomPosMap[bond->getAtomTo()];
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
            itemBondMap[bid] = mData.size();
        }
        mData.push_back(std::move(sym));
    }
    // 加环
    for (auto &ring:kekuleRings) {
        auto sym = HwBond::GetHwBond(JBondType::CircleBond);
//        auto sym = BondItem::GetBond(JBondType::CircleBond);
        std::vector<cv::Point2f> pts;
        std::unordered_set<size_t> aids;
        for (auto &bid:ring) {
            aids.insert(mol.getBondsMap().find(bid)->second->getAtomFrom());
            aids.insert(mol.getBondsMap().find(bid)->second->getAtomTo());
        }
        for (auto &aid:aids) {
            pts.push_back(atomPosMap[aid]);
        }
        sym->setVertices(pts);
        mData.push_back(std::move(sym));
    }
    float avgSize = 0;
    for (auto &sym:mData) {
        auto bBox = sym->getBoundingBox();
        if (!bBox)continue;
        avgSize += (bBox->width + bBox->height);
    }
    avgSize /= (2 * mData.size());
    // 添加弧线转折
    std::cout << "lineBondMap.size()=" << lineBondMap.size() << std::endl;
    std::cout << "avgSize=" << avgSize << std::endl;
    for (auto&[bid1, bid2]:lineBondMap) {
        auto item1 = std::static_pointer_cast<HwSingleBond>(mData[itemBondMap[bid1]]);
        auto item2 = std::static_pointer_cast<HwSingleBond>(mData[itemBondMap[bid2]]);
        size_t aid;
        std::unordered_set<size_t> tmp;
        std::vector<size_t> aids = {mol.getBondsMap().find(bid1)->second->getAtomFrom(),
                                    mol.getBondsMap().find(bid1)->second->getAtomTo(),
                                    mol.getBondsMap().find(bid2)->second->getAtomFrom(),
                                    mol.getBondsMap().find(bid2)->second->getAtomTo()};
        for (auto &id:aids) {
            if (notExist(tmp, id)) {
                tmp.insert(id);
            } else {
                aid = id;
                break;
            }
        }
        // aid is angle atom
        cv::Point2f anglePos = atomPosMap[aid];
        std::cout << "anglePos=" << anglePos << std::endl;
//        std::cout << "bbox1=" << item1->getBoundingBox().value() << std::endl;
//        std::cout << "bbox2=" << item2->getBoundingBox().value() << std::endl;
        const float threshDis = avgSize / 3.0;
        // 1、排除影响范围里的所有点
        cv::Point2f nearestPt, npt1, npt2;
        float minDis;
        auto cond = [&](const cv::Point2f &_pt) -> bool {
            const float curDis = getDistance2D(_pt, anglePos);
            bool res = (curDis > threshDis);
            if (res && curDis < minDis) {
                minDis = curDis;
                nearestPt = _pt;
            }
            return res;
        };
        minDis = std::numeric_limits<float>::max();
        item1->keepIf(cond);
        npt1 = nearestPt;

        minDis = std::numeric_limits<float>::max();
        item2->keepIf(cond);
        npt2 = nearestPt;

//        ShapeItem newItem1, newItem2;
        // 2、添加圆弧: npt1-anglePos-npt2

//        item1->addShapeItem(newItem1);
//        item2->addShapeItem(newItem2);
    }
    return avgSize;
}

static std::unordered_map<DetectorClasses, int> labels = {
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

void HwMol::dumpAsDarknet(const std::string &_imgPath, const std::string &_labelPath,
                          const size_t &_repeatTimes) {
    float avgSize = reloadHWData(0.1);
    float k = 50.0f / (std::max)(0.01f, avgSize);
    this->mulK(k, k);
    for (size_t i = 0; i < _repeatTimes; i++) {
        this->rotate(rand() % 360);
        auto bBox = this->getBoundingBox().value();
        const int minWidth = 8 + bBox.width, minHeight = 8 + bBox.height;
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        this->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        this->paintTo(img);
        std::string suffix = "_" + std::to_string(i);
        if (byProb(hwController->getRevertColorProb())) {// 反转颜色
            cv::bitwise_not(img, img);
        }
        cv::imwrite(_imgPath + suffix + ".jpg", img,
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
            float centX = bBox.x + bBox.width / 2, centY = bBox.y + bBox.height / 2;
//            if (notExist(labels, name)) {
//                labels.insert({name, beginLabel++});
//                std::cout << name << " " << beginLabel - 1 << std::endl;
//            }
            ofsm << labels[name] << " " << centX / minWidth << " " << centY / minHeight << " "
                 << bBox.width / minWidth << " " << bBox.height / minHeight << "\n";
        }
        ofsm.close();
    }
}

void HwMol::showOnScreen(const size_t &_repeatTimes) {
    float avgSize = reloadHWData(0.1);
    float k = 50.0f / (std::max)(0.01f, avgSize);
    this->mulK(k, k);
    for (size_t i = 0; i < _repeatTimes; i++) {
        this->rotate(rand() % 360);
        auto bBox = this->getBoundingBox().value();
        const int minWidth = 8 + bBox.width, minHeight = 8 + bBox.height;
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        this->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        this->paintTo(img);
        cv::imshow("MolHwItem::showOnScreen", img);
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
