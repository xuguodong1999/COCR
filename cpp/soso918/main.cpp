#include "utils.hpp"
#include "symbol.hpp"
#include "hwchar.hpp"
#include "chemparser.hpp"
#include "bond.hpp"
#include "mol.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
const int www = 480, hhh = 320;

void demoSingle();

void demoLine();

void demoCircle();

void demoText();

void demoRichText();

void demoSpecialText();

void demoBond();

void makeIcon();

void demoMol() {
    Mol::LoopOn(get_chonps_json_path(), "test");
}

//#include <fdeep/fdeep.hpp>
//#include <chrono>
//using namespace fdeep;
//using namespace chrono;
//int main() {
//    const auto model = fdeep::load_model("/tmp/fdeep_model.json");
//    const int w=256,h=256;
//    uint8_t*fuck=new uint8_t[w*h*3];
//    tensors input({tensor_from_bytes(fuck,w,h, 3)});
//    auto start = system_clock::now();
//    const auto result = model.predict(input);
//    auto end = system_clock::now();
//    auto duration = duration_cast<microseconds>(end - start);
//    std::cout << duration.count() * milliseconds::period::num / milliseconds::period::den<< std::endl;
//    std::cout << fdeep::show_tensor_shape(result.at(0).shape()) << std::endl;
//    std::cout << fdeep::show_tensor_shape(result.at(1).shape()) << std::endl;
//    std::cout << fdeep::show_tensor_shape(result.at(2).shape()) << std::endl;
//}
int main() {
#ifdef WIN32
    _putenv("BABEL_DATADIR=C:\\Users\\xgd\\install\\openbabel\\data");
#elif defined(unix)
    putenv("BABEL_DATADIR=/home/xgd/install/openbabel/share/openbabel/3.1.0");
#endif

//    demoLine();
//    demoCircle();
//    demoSingle();
//    demoRichText();
//    demoSpecialText();
//    demoBond();
//    makeIcon();
    demoMol();
    return 0;
}


void demoSingle() {
    srand(918);
    while (true) {
        cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
        auto tmp = HWChar::GetByIntLabel(1 + rand() % 182);
        tmp.resizeTo(200, 200);
        tmp.rotate(-10 + rand() % 20);
        tmp.moveCenterTo(cv::Point2f(www / 2, hhh / 2));
        tmp.resizeTo(100, 100, false);
        tmp.paintTo(img1);
        cv::imshow("1", img1);
        cv::waitKey(0);
    }
}

void demoLine() {
    srand(918);
    while (true) {
        cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
        auto line = HWChar::GetShape("line");
        line.castToLine(cv::Point2f(50, 50), cv::Point2f(50, 250));
        line.paintTo(img1);
        cv::imshow("1", img1);
        cv::waitKey(0);
    }
}

void demoCircle() {
    srand(918);
    while (true) {
        cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
        auto circle = HWChar::GetShape("circle");
        circle.castToCircle(cv::Point2f(400, 200), 100, 100);
        circle.paintTo(img1);
        cv::imshow("1", img1);
        cv::waitKey(0);
    }
}

void demoText() {
    srand(918);
    while (true) {
        cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
        Symbol hehe(makeNotedStringByChar("1234567890"));
        hehe.resizeTo(400, 100);
        hehe.moveCenterTo(cv::Point2f(www / 2, hhh / 2));
        hehe.paintTo(img1);
        cv::imshow("1", img1);
        cv::waitKey(0);
    }
}

void demoRichText() {
    srand(918);
    while (true) {
        cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
        Symbol::setSAngleK((rand() % 20 - 10) / 100.0);

        Symbol::setSNormSizeK((rand() % 10 - 5) / 100.0 + 1.0);
        Symbol::setSNormOffsetKx((rand() % 10 - 5) / 100.0);
        Symbol::setSNormOffsetKy((rand() % 10 - 5) / 100.0);

        Symbol::setSSubSizeK((rand() % 10 - 15) / 100.0 + 1.0);
        Symbol::setSSubOffsetKx((rand() % 10 - 5) / 100.0);
        Symbol::setSSubOffsetKy((rand() % 10 - 5) / 100.0);
        Symbol hehe(makeNotedString(
//                "CuSO4(5H2O)",
//                {Normal, RightButtom, Normal, Normal, RightButtom, Normal, Normal, Normal, RightButtom, Normal}
//                "SO42-",
//                {Normal, Normal, RightButtom, Up, RightTop}
                "CuSO4",
                {Normal, RightButtom, Normal, Normal, RightButtom}
        ));
        hehe.resizeTo(www - 20, hhh - 20);
        hehe.moveCenterTo(cv::Point2f(www / 2, hhh / 2));
        hehe.paintTo(img1);
        cv::imshow("1", img1);
        cv::waitKey(0);
    }
}

void demoSpecialText() {
    srand(918);
    while (true) {
        cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
//        auto positiveSym = Symbol::GetSymbol("positive");
        auto positiveSym = Symbol::GetSymbol("negative");
        positiveSym->resizeTo(200, 200);
        positiveSym->moveCenterTo(cv::Point2f(www / 2, hhh / 2));
        positiveSym->paintTo(img1);
//        cv::imwrite("D:/tmp.jpg",img1);
//        break;
        cv::imshow("1", img1);
        cv::waitKey(0);
    }
}

void demoBond() {
    srand(918);
    while (true) {
        cv::Mat img1 = cv::Mat(hhh, www, CV_8UC3, WHITE);
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
}

void makeIcon() {
    const std::string dir = "C:\\Users\\xgd\\source\\COCR\\android\\app\\src\\main\\res\\drawable\\";
    std::vector<std::string> sym = {
            "positive", "negative"};
//    for (auto &s:sym) {
//        cv::Mat img = cv::Mat(256, 256, CV_8UC3, WHITE);
//        auto tmp = Symbol::GetSymbol(s);
//        tmp->resizeTo(200, 200);
//        tmp->moveCenterTo(cv::Point2f(128,128));
//        tmp->paintTo(img);
//        cv::imwrite(dir + "icon_hint_" + s + ".png", img);
//    }
    sym = {"SolidWedge", "DashWedge", "Single", "Double", "Triple", "Wave"};
    for (auto &s:sym) {
        cv::Mat img = cv::Mat(256, 256, CV_8UC3, WHITE);
        auto tmp = Bond::GetBond(s);
        tmp->setUseHandWrittenWChar(true);
//        tmp->resizeTo(200, 200);
        tmp->setVertices({cv::Point2f(20, 20), cv::Point2f(196, 196)});
//        tmp->moveCenterTo(cv::Point2f(128,128));
        tmp->paintTo(img);
        cv::imwrite(dir + "icon_hint_" + s + ".png", img);
    }
}
