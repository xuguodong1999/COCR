#include "mol_hw.hpp"
#include "timer.hpp"
#include "colors.hpp"
#include <opencv2/opencv.hpp>
#include <filesystem>
#include <fstream>
#include <iomanip>

using namespace std;

MolHwItem::MolHwItem(const JMol &_jmol) : mol(_jmol) {

}

void MolHwItem::paintTo(cv::Mat &canvas) {
    for (auto &sym:symbols) {
        sym->paintTo(canvas);
    }
}

void MolHwItem::rotateBy(float angle, const cv::Point2f &cent) {
    for (auto &s:symbols) {
        s->rotateBy(angle, cent);
        if (!s->IsRotateAllowed()) {
            s->rotate(-angle);
        }
    }
}

const cv::Rect2f MolHwItem::getBoundingBox() const {
    if (symbols.empty()) {
        return cv::Rect2f(0, 0, 1, 1);
    }
    float minx, miny, maxx, maxy;
    minx = miny = std::numeric_limits<float>::max();
    maxx = maxy = std::numeric_limits<float>::min();
    for (auto &sym :symbols) {
        auto rect = sym->getBoundingBox();
        minx = std::min(minx, rect.x);
        miny = std::min(miny, rect.y);
        maxx = std::max(maxx, rect.x + rect.width);
        maxy = std::max(maxy, rect.y + rect.height);
    }
    return cv::Rect2f(minx, miny, maxx - minx, maxy - miny);
}

void MolHwItem::rotate(float angle) {
    // 旋转的逻辑，骨架直接旋转，字符只做平移
    auto cent = getCenter();
    for (auto &sym:symbols) {
        sym->rotateBy(angle, cent);
        if (!sym->IsRotateAllowed()) {
            sym->rotate(-angle);
        }
    }
}

void MolHwItem::move(const cv::Point2f &offset) {
    for (auto &sym:symbols) {
        sym->move(offset);
    }
}

void MolHwItem::moveCenterTo(const cv::Point2f &newCenter) {
    auto oldCenter = getCenter();
    cv::Point2f offset = newCenter - oldCenter;
    move(offset);
}

void MolHwItem::moveLeftTopTo(const cv::Point2f &leftTop) {
    cv::Rect2f bBox = getBoundingBox();
    cv::Point2f offset = bBox.tl() - leftTop;
    move(offset);
}

void MolHwItem::resizeTo(float w, float h, bool keepRatio) {
    cv::Rect2f bBox = getBoundingBox();
    cv::Point2f oldCenter(bBox.x + bBox.width / 2, bBox.y + bBox.height / 2);
    float oldW = bBox.width, oldH = bBox.height;
    float kx = w / oldW;
    float ky = h / oldH;
    moveLeftTopTo(cv::Point2f(0, 0));
    if (keepRatio) {
        kx = ky = std::min(kx, ky);
    }
    mulK(kx, ky);
    moveCenterTo(oldCenter);
}

void MolHwItem::mulK(float kx, float ky) {
    for (auto &s:symbols) {
        s->mulK(kx, ky);
    }
}

float MolHwItem::reloadHWData(const float &_showCProb) {
    symbols.clear();
    // 记录显式写出的原子 id
    std::unordered_map<size_t, bool> explicitAtomMap;
    auto atomPosMap = mol.get2DCoordinates();
    for (auto&[id, atom]:mol.getAtomsMap()) {
        auto sym = ShapeGroup::GetShapeGroup(atom->getElementName());
        float fontSize = mol.getFontSize() * betweenProb(0.3, 0.7);
        sym->resizeTo(fontSize, fontSize);
        sym->moveCenterTo(atomPosMap[id]);
        if (ElementType::C == atom->getElementType()) {
            explicitAtomMap[id] = byProb(_showCProb);
        } else {
            explicitAtomMap[id] = true;
        }
        if (explicitAtomMap[id]) {
            symbols.push_back(std::move(sym));
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
    for (auto&[id, bond]:mol.getBondsMap()) {
        JBondType bondType = bond->getBondType();
        if (bondInKekuleRings.end() != bondInKekuleRings.find(id)) {
            bondType = JBondType::DelocalizedBond;  // 不能影响 JMol 里的数据
        }
        shared_ptr<BondItem> sym = BondItem::GetBond(bondType);
        sym->setUseHandWrittenWChar(true);
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
        symbols.push_back(std::move(sym));
    }
    for (auto &ring:kekuleRings) {
        auto sym = BondItem::GetBond(JBondType::CircleBond);
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
        symbols.push_back(std::move(sym));
    }
    float avgSize = 0;
    for (auto &sym:symbols) {
        auto bBox = sym->getBoundingBox();
        avgSize += (bBox.width + bBox.height);
    }
    avgSize /= (2 * symbols.size());
    return avgSize;
}

static std::unordered_map<std::string, int> labels = {
        {"O",      1},
        {"I",      2},
        {"C",      6},
        {"F",      12},
        {"N",      5},
        {"H",      4},
        {"Dash",   9},
        {"Single", 8},
        {"Double", 11},
        {"Triple", 13},
        {"Circle", 16},
        {"S",      3},
        {"P",      15},
        {"Br",     0},
        {"B",      7},
        {"Solid",  10},
        {"Cl",     14}};
//static int beginLabel = 0;

void MolHwItem::dumpAsDarknet(const std::string &_imgPath, const std::string &_labelPath,
                              const size_t &_repeatTimes) {
    float avgSize = reloadHWData(0.1);
    float k = 50.0f / (std::max)(0.01f, avgSize);
    this->mulK(k, k);
    for (size_t i = 0; i < _repeatTimes; i++) {
        this->rotate(rand() % 360);
        auto bBox = this->getBoundingBox();
        const int minWidth = 8 + bBox.width, minHeight = 8 + bBox.height;
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        this->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        this->paintTo(img);
        std::string suffix = "_" + std::to_string(i);
        if (byProb(RC::revertColorProb)) {// 反转颜色
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
        for (auto &sym:symbols) {
            auto &name = sym->getName();
            auto bBox = sym->getBoundingBox();
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

void MolHwItem::showOnScreen(const size_t &_repeatTimes) {
    float avgSize = reloadHWData(0.1);
    float k = 50.0f / (std::max)(0.01f, avgSize);
    this->mulK(k, k);
    for (size_t i = 0; i < _repeatTimes; i++) {
        this->rotate(rand() % 360);
        auto bBox = this->getBoundingBox();
        const int minWidth = 8 + bBox.width, minHeight = 8 + bBox.height;
        cv::Mat img = cv::Mat(minHeight, minWidth, CV_8UC3,
                              getScalar(ColorName::rgbWhite));
        this->moveCenterTo(cv::Point2f(minWidth / 2, minHeight / 2));
        this->paintTo(img);
        cv::imshow("MolHwItem::showOnScreen", img);
        cv::waitKey(0);
    }
}
