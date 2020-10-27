#include "config.hpp"
#include "bonditem.hpp"
#include "molitem.hpp"
#include "couch.hpp"
#include <string>
#include <exception>
#include <iostream>
#include <iomanip>
#include <opencv2/opencv.hpp>


const int www = 480, hhh = 320;

void demoSingle() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    auto tmp = CouchItem::GetByIntLabel(1 + rand() % 182);
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
    auto line = CouchItem::GetShape("line");
    line.castToLine(cv::Point2f(50, 50), cv::Point2f(50, 250));
    line.paintTo(img1);
    cv::imshow("1", img1);
    cv::waitKey(0);
}

void demoCircle() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    auto circle = CouchItem::GetShape("circle");
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
//    MolItem::LoopOn(get_coordinates_json_path(0), "testDraw");
}

void demoBond() {
    cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, cvWhite);
    auto bond = BondItem::GetBond("DashWedge");
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
        auto tmp = BondItem::GetBond(s);
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

void loop(function<void(void)> demoFunc) {
    srand(918);
    while (true) { demoFunc(); }
}


int main(int argc, char **argv) {
//    demo_opencv();
//    demo_torch();
//    std::cout<<demo_openbabel()<<std::endl;
//    loop(demoMol);
    demoMol();


    return 0;
}
