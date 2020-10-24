#include "config.hpp"

#include <string>
#include <exception>
#include <iostream>
#include <iomanip>

void demo_torch();

void demo_opencv();

std::string demo_openbabel(const char *smi = "c1ccccc1");

void demoSingle();

void demoLine();

void demoCircle();

void demoSymbol();

void demoRichSymbol();

void demoSpecialSymbol();

void demoBond();

void makeIcon();

void demoMol();

void loop(f<void(void)> demoFunc) {
    srand(918);
    while (true) { demoFunc(); }
}

void trainSimpleClassifier();

void testSwish();

inline void put_env() {
#ifdef WIN32
    _putenv(const_cast<char *>("BABEL_DATADIR=C:/static/openbabel-3.1.1/data"));
#elif defined(unix)
    putenv(const_cast<char*>("BABEL_DATADIR=/home/xgd/install/openbabel/share/openbabel/3.1.0"));
#endif
}

#include "isomer.hpp"
#include <sstream>
#include "polya.hpp"

int main(int argc, char **argv) {
    put_env();
//    demo_opencv();
//    demo_torch();
//    std::cout<<demo_openbabel()<<std::endl;
//    loop(demoMol);
//    demoMol();

//    trainSimpleClassifier();

    // 结构生成
    //auto &IC = IsomerCounter::GetInstance();
    //IC.calculate(27, "D:/alkane");
    //IC.calculate_i_from_i_1( "D:/alkane", 28);

    // 波利亚计数法
    auto &ic = PolyaIsomerCounter::GetInstance();
    ic.count(32);
    return 0;

}
