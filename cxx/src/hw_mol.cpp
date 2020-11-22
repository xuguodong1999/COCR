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


//void MolItem::testRing() {
//    for (auto it = obMol.BeginAtoms(); it != obMol.EndAtoms(); it++) {
//        std::cout << RC::sElementData[(*it)->GetAtomicNum()] << ","
//                  << (*it)->GetId() << "," << (*it)->GetIdx() << ",("
//                  << (*it)->GetX() << "," << (*it)->GetY() << ")" << std::endl;
//    }
//    for (auto it = obMol.BeginBonds(); it != obMol.EndBonds(); it++) {
//        // bond.atom.idx 根据 atom.idx
//        std::cout << "(" << (*it)->GetBeginAtomIdx() << ","
//                  << (*it)->GetEndAtomIdx() << "),"
//                  // bond 索引，用不到
//                  << (*it)->GetId() << "," << (*it)->GetIdx() << ","
//                  // 键级，（实楔形键，虚楔形键，在环中）
//                  << (*it)->GetBondOrder() << ",(" << (*it)->IsWedge() << ","
//                  << (*it)->IsHash() << "," << (*it)->IsInRing() << ")"
//                  << std::endl;
//    }
//    auto rings = obMol.GetSSSR();
//    for (auto &ring:rings) {
//        auto aids = ring->_path; // 标注 atom.idx
//        std::cout << "ring->IsAromatic()=" << ring->IsAromatic() << std::endl;
//        for (auto &a:aids) {
//            std::cout << a << ",";
//        }
//        std::cout << std::endl;
//    }
//}




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

//void MolItem::LoopOn(const char *filename, const string &taskName) {
//    std::ifstream ifsm(filename);
//    rapidjson::Document doc;
//    string buffer;
//    if (!ifsm.is_open()) {
//        std::cerr << "fail to open " << filename << std::endl;
//        exit(-1);
//    }
//    ifsm.seekg(0, std::ios::end);
//    auto fileSize = ifsm.tellg();
//    ifsm.seekg(0, std::ios::beg);
//    buffer.resize(fileSize);
//    ifsm.read(const_cast<char *>(buffer.data()), fileSize);
//    ifsm.close();
//    buffer = boost::algorithm::replace_all_copy(buffer, "\\", "\\\\");
//    doc.Parse(buffer.c_str());
//    std::cout << "ParseErrorCode=" << doc.GetParseError() << std::endl
//              << "ErrorOffset=" << doc.GetErrorOffset() << std::endl
//              << "HasParseError=" << doc.HasParseError() << std::endl
//              << "count=" << doc.GetArray().Size() << std::endl;
//    for (const auto &obj:doc.GetArray()) {
//        const auto smiles = obj["smiles"].GetString();
//        Mol mol;
//        mol.inSmiles = std::move(smiles);
//        const auto atoms = obj["atoms"].GetArray();
//        const auto bonds = obj["bonds"].GetArray();
//        for (const auto &atomObj : atoms) {
//            auto &&atom = atomObj.GetObject();
//            mol.addAtom(atom["id"].GetInt(),
//                        atom["x"].GetFloat(), atom["y"].GetFloat(),
//                        atom["element"].GetString(),
//                        atom["charge"].GetInt());
//            RC::aeSet.insert(atom["element"].GetString());
//            RC::acSet.insert(atom["charge"].GetInt());
//        }
//        for (const auto &bondObj : bonds) {
//            auto &&bond = bondObj.GetObject();
//            mol.addBond(bond["from"].GetInt(), bond["to"].GetInt(),
//                        bond["type"].GetString(),
//                        bond["stereo"].GetString());
//            RC::btSet.insert(bond["type"].GetString());
//            RC::bsSet.insert(bond["stereo"].GetString());
//        }
//        mol.run(taskName);
//    }
//}

void MolItem::addAtomItem(int id, float x, float y, const string &element, int charge) {
//    OpenBabel::OBAtom obatom;
//    auto it = std::find(RC::sElementData.begin(), RC::sElementData.end(), element);
//    obatom.SetId(id);
//    obatom.SetAtomicNum(std::distance(RC::sElementData.begin(), it));
//    obatom.SetFormalCharge(charge);
//    obatom.SetVector(x, y, 0);
////    std::cout << x << "," << y << std::endl;
//    obatom.SetCoordPtr(nullptr);
//    obMol.AddAtom(obatom, false);
//    aidMap[id] = obMol.GetAtomById(id);
}

void MolItem::addBondItem(int from, int to, const string &type, const string &stereo) {
//    OpenBabel::OBBond obbond;
//    obbond.SetBegin(aidMap[from]);
//    obbond.SetEnd(aidMap[to]);
//    if (stereo == "Up") {
//        obbond.SetWedge(true);
//    } else if (stereo == "Down") {
//        obbond.SetHash(true);
//    }
//    if (type == "Single") {
//        obbond.SetBondOrder(1);
//    } else if (type == "Double") {
//        obbond.SetBondOrder(2);
//    } else if (type == "Triple") {
//        obbond.SetBondOrder(3);
//    }
//    obMol.AddBond(obbond);
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

//void MolItem::testDraw() {
//    symbols.clear();
//    for (auto it = obMol.BeginAtoms(); it != obMol.EndAtoms(); it++) {
//        auto sym = ShapeGroup::GetShapeGroup(RC::sElementData[(*it)->GetAtomicNum()]);
//        // FIXME: 交换下面两句，字符坐标有偏差，这不符合 API 的行为约定
//        sym->resizeTo(5, 5);
//        sym->moveCenterTo(cv::Point2f((*it)->GetX(), (*it)->GetY()));
//        symbols.push_back(std::move(sym));
//    }
//    for (auto it = obMol.BeginBonds(); it != obMol.EndBonds(); it++) {
//        // bond.atom.idx 根据 atom.idx
//        shared_ptr<Bond> sym;
//        if ((*it)->GetBondOrder() == 1) {
//            if ((*it)->IsWedge()) {
//                sym = Bond::GetBond("SolidWedge");
//            } else if ((*it)->IsHash()) {
//                sym = Bond::GetBond("DashWedge");
//            } else {
//                sym = Bond::GetBond("Single");
//            }
//        } else if ((*it)->GetBondOrder() == 2) {
//            sym = Bond::GetBond("Double");
//        } else if ((*it)->GetBondOrder() == 3) {
//            sym = Bond::GetBond("Triple");
//        }
//        sym->setUseHandWrittenWChar(true);
//        sym->setVertices(
//                {cv::Point2f((*it)->GetBeginAtom()->GetX(), (*it)->GetBeginAtom()->GetY()),
//                 cv::Point2f((*it)->GetEndAtom()->GetX(), (*it)->GetEndAtom()->GetY())
//                });
//        symbols.push_back(std::move(sym));
//    }
//    const int www = 1256, hhh = 640;
//    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
//    this->rotate(rand() % 360);
//    this->resizeTo(www - 20, hhh - 20);
//    this->moveCenterTo(cv::Point2f(www / 2, hhh / 2));
//    this->paintTo(img1);
//    for (auto &ss:symbols) {
//        cv::rectangle(img1, ss->getBoundingBox(), cvRed, 1);
//    }
//    cv::imshow("testDraw", img1);
//    cv::waitKey(0);
//}

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
        }else{
            explicitAtomMap[id] =true;
        }
        if(explicitAtomMap[id]) {
            symbols.push_back(std::move(sym));
        }
    }
    for (auto&[id, bond]:mol.getBondsMap()) {
        shared_ptr<BondItem> sym;
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
        cv::Point2f from=from_,to=to_;
        const float k=0.3;
        if(explicitAtomMap[bond->getAtomFrom()]){
            from=(1-k)*(from_-to_)+to_;
        }
        if(explicitAtomMap[bond->getAtomTo()]){
            to=(1-k)*(to_-from_)+from_;
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