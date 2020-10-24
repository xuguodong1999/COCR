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

void loop(function<void(void)> demoFunc) {
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

#include "generator.hpp"

int main(int argc, char **argv) {
    put_env();
//    demo_opencv();
//    demo_torch();
//    std::cout<<demo_openbabel()<<std::endl;
//    loop(demoMol);
//    demoMol();

//    trainSimpleClassifier();


    // 读取烷烃异构体并生成SMILES串、添加杂原子和基团修饰
    const char *alkane_dir = "C:/Users/xgd/source/COCR/data/alkane";
    auto &sg = SMILESGenerator::GetInstance();
    for (int i = 19; i <= 26; i++) {
        sg.HashToRichSMILES(alkane_dir, i);
    }
    return 0;

}
