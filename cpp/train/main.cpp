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


int main(int argc, char **argv) {
#ifdef WIN32
    _putenv("BABEL_DATADIR=C:/StaticLibs/openbabel-3.1.1/data");
#elif defined(unix)
    putenv("BABEL_DATADIR=/home/xgd/install/openbabel/share/openbabel/3.1.0");
#endif
//    demo_opencv();
//    demo_torch();
//    std::cout<<demo_openbabel()<<std::endl;
//    loop(demoMol);
//    demoMol();


        trainSimpleClassifier();

    return 0;
}
