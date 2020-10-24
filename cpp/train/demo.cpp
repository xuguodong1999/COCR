#include "config.hpp"
#include "shape.hpp"
#include "bond.hpp"
#include "shapegroup.hpp"
#include "handwritting.hpp"
#include "mol.hpp"

#include <opencv2/opencv.hpp>

#include <openbabel/mol.h>
#include <openbabel/builder.h>
#include <openbabel/forcefield.h>
#include <openbabel/obconversion.h>


#include "activation.hpp"

void testSwish() {
    torch::Tensor x = torch::tensor(0.5);
    MemoryEfficientSwish activation;
    auto y = activation->forward(x);
    std::cout << "y=" << y << std::endl;
    y = activation->forward(x);
    std::cout << "y=" << y << std::endl;
}

const int www = 480, hhh = 320;

void demoSingle() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    auto tmp = HandWritting::GetByIntLabel(1 + rand() % 182);
    tmp.resizeTo(200, 200);
    tmp.rotate(-10 + rand() % 20);
    tmp.moveCenterTo(cv::Point2f(www / 2, hhh / 2));
    tmp.resizeTo(100, 100, false);
    tmp.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoLine() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    auto line = HandWritting::GetShape("line");
    line.castToLine(cv::Point2f(50, 50), cv::Point2f(50, 250));
    line.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoCircle() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    auto circle = HandWritting::GetShape("circle");
    circle.castToCircle(cv::Point2f(400, 200), 100, 100);
    circle.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoShapeGroup() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    ShapeGroup hehe(makeNotedStringByChar("1234567890"));
    hehe.resizeTo(400, 100);
    hehe.moveCenterTo(cv::Point2f(www / 2, hhh / 2));
    hehe.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoRichShapeGroup() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    ShapeGroup::setSAngleK((rand() % 20 - 10) / 100.0);
    ShapeGroup::setSNormSizeK((rand() % 10 - 5) / 100.0 + 1.0);
    ShapeGroup::setSNormOffsetKx((rand() % 10 - 5) / 100.0);
    ShapeGroup::setSNormOffsetKy((rand() % 10 - 5) / 100.0);
    ShapeGroup::setSSubSizeK((rand() % 10 - 15) / 100.0 + 1.0);
    ShapeGroup::setSSubOffsetKx((rand() % 10 - 5) / 100.0);
    ShapeGroup::setSSubOffsetKy((rand() % 10 - 5) / 100.0);
    ShapeGroup hehe(makeNotedString(
//                "CuSO4(5H2O)",
//                {Normal, RightButtom, Normal, Normal, RightButtom, Normal, Normal, Normal, RightButtom, Normal}
//                "SO42-",
//                {Normal, Normal, RightButtom, Up, RightTop}
            "W",
            {Normal}
//                "CuSO4",
//                {Normal, RightButtom, Normal, Normal, RightButtom}
    ));
    hehe.resizeTo(50, 50);
    hehe.moveLeftTopTo(cv::Point2f(0, 0));
//        hehe.moveCenterTo(cv::Point2f(25,25));
    hehe.rotate(180);
//        hehe.moveCenterTo(cv::Point2f(www / 2, hhh / 2));
//        hehe.rotate(90);
//        hehe.resizeTo(www /3, hhh /3);
    hehe.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoSpecialShapeGroup() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
//        auto positiveSym = ShapeGroup::GetShapeGroup("positive");
    auto positiveSym = ShapeGroup::GetShapeGroup("negative");
    positiveSym->resizeTo(200, 200);
    positiveSym->moveCenterTo(cv::Point2f(www / 2, hhh / 2));
    positiveSym->paintTo(img1);
//        cv::imwrite("D:/tmp.jpg",img1);
//        break;
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoMol() {
    Mol::LoopOn(get_coordinates_json_path(0), "testDraw");
}

void demoBond() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    auto bond = Bond::GetBond("DashWedge");
    bond->setUseHandWrittenWChar(true);
    bond->setVertices({cv::Point2f(20, 20),
                       cv::Point2f(www * 0.6, hhh * 0.6)});
    bond->paintTo(img1);
//        cv::imwrite("D:/tmp.jpg", img1);
//        break;
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void makeIcon() {
    const string dir = "C:\\Users\\xgd\\source\\COCR\\android\\app\\src\\main\\res\\drawable\\";
    vector<string> sym = {
            "positive", "negative"};
//    for (auto &s:sym) {
//        cv::Mat img = cv::Mat(256, 256, CV_8UC3, WHITE);
//        auto tmp = ShapeGroup::GetShapeGroup(s);
//        tmp->resizeTo(200, 200);
//        tmp->moveCenterTo(cv::Point2f(128,128));
//        tmp->paintTo(img);
//        cv::imwrite(dir + "icon_hint_" + s + ".png", img);
//    }
    sym = {"SolidWedge", "DashWedge", "Single", "Double", "Triple", "Wave"};
    for (auto &s:sym) {
        cv::Mat img = cv::Mat(256, 256, CV_8UC3, cvWhite);
        auto tmp = Bond::GetBond(s);
        tmp->setUseHandWrittenWChar(true);
//        tmp->resizeTo(200, 200);
        tmp->setVertices({cv::Point2f(20, 20), cv::Point2f(196, 196)});
//        tmp->moveCenterTo(cv::Point2f(128,128));
        tmp->paintTo(img);
        cv::imwrite(dir + "icon_hint_" + s + ".png", img);
    }
}


void demo_opencv() {
    auto img = cv::imread("C:/Users/xgd/Pictures/workspace/xiaoqingwa.png");
    cv::imshow("demo", img);
    cv::waitKey(0);
}

#define RET_EMPTY_STRING_IF_NOT(condition) if(!(condition))return ""

string demo_openbabel(const char *smi) {
    using namespace OpenBabel;
    using namespace std;
    OBConversion conv;
    try {
        auto format_in = conv.FindFormat("smiles");
        auto format_out = conv.FindFormat("pdb");
        stringstream ism(smi);
        OBMol mol;
        RET_EMPTY_STRING_IF_NOT(conv.SetInFormat(format_in));
        RET_EMPTY_STRING_IF_NOT(conv.SetOutFormat(format_out));
        RET_EMPTY_STRING_IF_NOT(conv.Read(&mol, &ism));
        auto pFF = OBForceField::FindForceField("UFF");
        RET_EMPTY_STRING_IF_NOT(pFF);
        OBBuilder builder;
        RET_EMPTY_STRING_IF_NOT(builder.Build(mol));
        RET_EMPTY_STRING_IF_NOT(mol.AddHydrogens());
        // FIXME: ret value in OBForceField::Setup is false?
        pFF->Setup(mol);
        RET_EMPTY_STRING_IF_NOT(pFF->UpdateCoordinates(mol));
        stringstream osm;
        RET_EMPTY_STRING_IF_NOT(conv.Write(&mol, &osm));
        return std::move(osm.str());
    } catch (exception e) {
        return "";
    }
}
