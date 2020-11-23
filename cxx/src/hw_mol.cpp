#include "hw_mol.hpp"
#include "hw.hpp"
#include "rdkit_util.hpp"
#include <opencv2/opencv.hpp>
#include <set>

using namespace std;

MolItem::MolItem(const JMol &_jmol) : mol(_jmol) {
    if (mol.getAtomPosMap().empty()) {
        std::cerr << "You must call JMol::update2DCoordinates before use MolItem"
                  << std::endl;
        exit(-1);
    }
}

void MolItem::paintTo(cv::Mat &canvas) {
    for (auto &sym:symbols) {
        sym->paintTo(canvas);
    }
}

void MolItem::rotateBy(float angle, const cv::Point2f &cent) {
    for (auto &s:symbols) {
        s->rotateBy(angle, cent);
        if (!s->IsRotateAllowed()) {
            s->rotate(-angle);
        }
    }
}

const cv::Rect2f MolItem::getBoundingBox() const {
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

void MolItem::rotate(float angle) {
    // 旋转的逻辑，骨架直接旋转，字符只做平移
    auto cent = getCenter();
    for (auto &sym:symbols) {
        sym->rotateBy(angle, cent);
        if (!sym->IsRotateAllowed()) {
            sym->rotate(-angle);
        }
    }
}

void MolItem::move(const cv::Point2f &offset) {
    for (auto &sym:symbols) {
        sym->move(offset);
    }
}

void MolItem::moveCenterTo(const cv::Point2f &newCenter) {
    auto oldCenter = getCenter();
    cv::Point2f offset = newCenter - oldCenter;
    move(offset);
}

void MolItem::moveLeftTopTo(const cv::Point2f &leftTop) {
    cv::Rect2f bBox = getBoundingBox();
    cv::Point2f offset = bBox.tl() - leftTop;
    move(offset);
}

void MolItem::resizeTo(float w, float h, bool keepRatio) {
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

void MolItem::mulK(float kx, float ky) {
    for (auto &s:symbols) {
        s->mulK(kx, ky);
    }
}

void MolItem::reloadHWData(const float &_showCProb) {
    symbols.clear();
    // 记录显式写出的原子 id
    std::unordered_map<size_t, bool> explicitAtomMap;
    for (auto&[id, atom]:mol.getAtomsMap()) {
        auto sym = ShapeGroup::GetShapeGroup(atom->getElementName());
        // FIXME: 交换下面两句，字符坐标有偏差，这不符合 API 的行为约定
        sym->resizeTo(1, 1);// 这个比例和RDKit的绘图引擎关联
        sym->moveCenterTo(mol.getAtomPosMap().at(id));
        if (ElementType::C == atom->getElementType()) {
            explicitAtomMap[id] = byProb(_showCProb);
        } else {
            explicitAtomMap[id] = true;
        }
        if (explicitAtomMap[id]) {
            symbols.push_back(std::move(sym));
        }
    }
    for (auto&[id, bond]:mol.getBondsMap()) {
        shared_ptr < BondItem > sym;
        switch (bond->getBondType()) {
            case JBondType::SolidWedgeBond:
                sym = BondItem::GetBond("SolidWedge");
                break;
            case JBondType::DashWedgeBond:
                sym = BondItem::GetBond("DashWedge");
                break;
            case JBondType::WaveBond:
                sym = BondItem::GetBond("Wave");
                break;
            case JBondType::SingleBond :
                sym = BondItem::GetBond("Single");
                break;
            case JBondType::DoubleBond:
                sym = BondItem::GetBond("Double");
                break;
            case JBondType::TripleBond:
                sym = BondItem::GetBond("Triple");
                break;
            case JBondType::DelocalizedBond:
                // FIXME ADD CIRCLE BOND
                std::cerr << "Item for JBondType::DelocalizedBond not implemented" << std::endl;
                exit(-1);
                break;
            default: {
                std::cerr << "convert " << static_cast<int>(bond->getBondType())
                          << " To RDKitBondType failed " << std::endl;
                exit(-1);
            }
        }
        sym->setUseHandWrittenWChar(true);
        auto from_ = mol.getAtomPosMap().at(bond->getAtomFrom());
        auto to_ = mol.getAtomPosMap().at(bond->getAtomTo());
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
    const int www = 1256, hhh = 640;
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    this->rotate(rand() % 360);
    this->resizeTo(www - 20, hhh - 20);
    this->moveCenterTo(cv::Point2f(www / 2, hhh / 2));
    this->paintTo(img1);
    for (auto &ss:symbols) {
        cv::rectangle(img1, ss->getBoundingBox(), cvRed, 1);
    }
    cv::imshow("testDraw", img1);
    cv::waitKey(0);
}