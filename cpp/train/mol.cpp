#include "mol.hpp"
#include "bond.hpp"
#include "statistic.hpp"
#include <openbabel/ring.h>
#include <openbabel/obconversion.h>
#include <opencv2/opencv.hpp>
#include <boost/algorithm/string.hpp>
#include <rapidjson/document.h>
#include <set>

Mol::Mol() {
}

Mol::~Mol() {
}

s Mol::getStandardSMILES() {
    OpenBabel::OBConversion obConversion;
    obConversion.SetOutFormat("can");
    std::stringstream ism;
    obConversion.Write(&obMol, &ism);
    return std::move(ism.str());
}

void Mol::testRing() {
    for (auto it = obMol.BeginAtoms(); it != obMol.EndAtoms(); it++) {
        std::cout << RC::sElementData[(*it)->GetAtomicNum()] << ","
                  << (*it)->GetId() << "," << (*it)->GetIdx() << ",("
                  << (*it)->GetX() << "," << (*it)->GetY() << ")" << std::endl;
    }
    for (auto it = obMol.BeginBonds(); it != obMol.EndBonds(); it++) {
        // bond.atom.idx 根据 atom.idx
        std::cout << "(" << (*it)->GetBeginAtomIdx() << ","
                  << (*it)->GetEndAtomIdx() << "),"
                  // bond 索引，用不到
                  << (*it)->GetId() << "," << (*it)->GetIdx() << ","
                  // 键级，（实楔形键，虚楔形键，在环中）
                  << (*it)->GetBondOrder() << ",(" << (*it)->IsWedge() << ","
                  << (*it)->IsHash() << "," << (*it)->IsInRing() << ")"
                  << std::endl;
    }
    auto rings = obMol.GetSSSR();
    for (auto &ring:rings) {
        auto aids = ring->_path; // 标注 atom.idx
        std::cout << "ring->IsAromatic()=" << ring->IsAromatic() << std::endl;
        for (auto &a:aids) {
            std::cout << a << ",";
        }
        std::cout << std::endl;
    }
}

void Mol::run(const s &taskName) {
    if (taskName == "testDraw") {
        testDraw();
        return;
    } else if (taskName == "testRing") {
        testRing();
    } else if (taskName == "log") {
        std::cout << "input=" << inSmiles << std::endl;
        std::cout << "final=" << getStandardSMILES() << std::endl;
    }
#ifdef WIN32
    system("pause");
#else
    std::cin.get();
#endif
}

void Mol::paintTo(cv::Mat &canvas) {
    for (auto &sym:symbols) {
        sym->paintTo(canvas);
    }
}

void Mol::rotateBy(float angle, const cv::Point2f &cent) {
    for (auto &s:symbols) {
        s->rotateBy(angle, cent);
        if (!s->IsRotateAllowed()) {
            s->rotate(-angle);
        }
    }
}

void Mol::LoopOn(const char *filename, const s &taskName) {
    std::ifstream ifsm(filename);
    rapidjson::Document doc;
    s buffer;
    if (!ifsm.is_open()) {
        std::cerr << "fail to open " << filename << std::endl;
        exit(-1);
    }
    ifsm.seekg(0, std::ios::end);
    auto fileSize = ifsm.tellg();
    ifsm.seekg(0, std::ios::beg);
    buffer.resize(fileSize);
    ifsm.read(const_cast<char *>(buffer.data()), fileSize);
    ifsm.close();
    buffer = boost::algorithm::replace_all_copy(buffer, "\\", "\\\\");
    doc.Parse(buffer.c_str());
    std::cout << "ParseErrorCode=" << doc.GetParseError() << std::endl
              << "ErrorOffset=" << doc.GetErrorOffset() << std::endl
              << "HasParseError=" << doc.HasParseError() << std::endl
              << "count=" << doc.GetArray().Size() << std::endl;
    int ii = 0;
    for (const auto &obj:doc.GetArray()) {
        const auto smiles = obj["smiles"].GetString();
        Mol mol;
        mol.inSmiles = std::move(smiles);
        const auto atoms = obj["atoms"].GetArray();
        const auto bonds = obj["bonds"].GetArray();
        for (const auto &atomObj : atoms) {
            auto &&atom = atomObj.GetObject();
            mol.addAtom(atom["id"].GetInt(),
                        atom["x"].GetFloat(), atom["y"].GetFloat(),
                        atom["element"].GetString(),
                        atom["charge"].GetInt());
            RC::aeSet.insert(atom["element"].GetString());
            RC::acSet.insert(atom["charge"].GetInt());
        }
        for (const auto &bondObj : bonds) {
            auto &&bond = bondObj.GetObject();
            mol.addBond(bond["from"].GetInt(), bond["to"].GetInt(),
                        bond["type"].GetString(),
                        bond["stereo"].GetString());
            RC::btSet.insert(bond["type"].GetString());
            RC::bsSet.insert(bond["stereo"].GetString());
        }
        mol.run(taskName);
        std::cout << ii++ << ",";
    }
}

void Mol::addAtom(int id, float x, float y, const s &element, int charge) {
    OpenBabel::OBAtom obatom;
    auto it = std::find(RC::sElementData.begin(), RC::sElementData.end(), element);
    obatom.SetId(id);
    obatom.SetAtomicNum(std::distance(RC::sElementData.begin(), it));
    obatom.SetFormalCharge(charge);
    obatom.SetVector(x, y, 0);
//    std::cout << x << "," << y << std::endl;
    obatom.SetCoordPtr(nullptr);
    obMol.AddAtom(obatom, false);
    aidMap[id] = obMol.GetAtomById(id);
}

void Mol::addBond(int from, int to, const s &type, const s &stereo) {
    OpenBabel::OBBond obbond;
    obbond.SetBegin(aidMap[from]);
    obbond.SetEnd(aidMap[to]);
    if (stereo == "Up") {
        obbond.SetWedge(true);
    } else if (stereo == "Down") {
        obbond.SetHash(true);
    }
    if (type == "Single") {
        obbond.SetBondOrder(1);
    } else if (type == "Double") {
        obbond.SetBondOrder(2);
    } else if (type == "Triple") {
        obbond.SetBondOrder(3);
    }
    obMol.AddBond(obbond);
}

const cv::Rect2f Mol::getBoundingBox() const {
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

void Mol::rotate(float angle) {
    // 旋转的逻辑，骨架直接旋转，字符只做平移
    auto cent = getCenter();
    for (auto &sym:symbols) {
        sym->rotateBy(angle, cent);
        if (!sym->IsRotateAllowed()) {
            sym->rotate(-angle);
        }
    }
}

void Mol::move(const cv::Point2f &offset) {
    for (auto &sym:symbols) {
        sym->move(offset);
    }
}

void Mol::moveCenterTo(const cv::Point2f &newCenter) {
    auto oldCenter = getCenter();
    cv::Point2f offset = newCenter - oldCenter;
    move(offset);
}

void Mol::moveLeftTopTo(const cv::Point2f &leftTop) {
    cv::Rect2f bBox = getBoundingBox();
    cv::Point2f offset = bBox.tl() - leftTop;
    move(offset);
}

void Mol::resizeTo(float w, float h, bool keepRatio) {
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

void Mol::testDraw() {
    symbols.clear();
    for (auto it = obMol.BeginAtoms(); it != obMol.EndAtoms(); it++) {
        auto sym = ShapeGroup::GetShapeGroup(RC::sElementData[(*it)->GetAtomicNum()]);
        // FIXME: 交换下面两句，字符坐标有偏差，这不符合 API 的行为约定
        sym->resizeTo(5, 5);
        sym->moveCenterTo(cv::Point2f((*it)->GetX(), (*it)->GetY()));
        symbols.push_back(std::move(sym));
    }
    for (auto it = obMol.BeginBonds(); it != obMol.EndBonds(); it++) {
        // bond.atom.idx 根据 atom.idx
        p <Bond> sym;
        if ((*it)->GetBondOrder() == 1) {
            if ((*it)->IsWedge()) {
                sym = Bond::GetBond("SolidWedge");
            } else if ((*it)->IsHash()) {
                sym = Bond::GetBond("DashWedge");
            } else {
                sym = Bond::GetBond("Single");
            }
        } else if ((*it)->GetBondOrder() == 2) {
            sym = Bond::GetBond("Double");
        } else if ((*it)->GetBondOrder() == 3) {
            sym = Bond::GetBond("Triple");
        }
        sym->setVertices(
                {cv::Point2f((*it)->GetBeginAtom()->GetX(), (*it)->GetBeginAtom()->GetY()),
                 cv::Point2f((*it)->GetEndAtom()->GetX(), (*it)->GetEndAtom()->GetY())
                });
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
//    cv::imshow("1", img1);
//    cv::waitKey(1);
}

void Mol::mulK(float kx, float ky) {
    for (auto &s:symbols) {
        s->mulK(kx, ky);
    }
}