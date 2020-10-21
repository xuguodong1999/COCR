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

int main(int argc, char **argv) {
    put_env();
//    demo_opencv();
//    demo_torch();
//    std::cout<<demo_openbabel()<<std::endl;
//    loop(demoMol);
//    demoMol();

//    trainSimpleClassifier();
//    auto ag=AlkaneGraph<unsigned char>::GetInstance();
//    ag.push_back(0,1);
//    ag.push_back(1,2);
//    ag.push_back(2,3);
//    //212
//    //10001010
//    //100010011
//    //11010100
//    //( () () () )
//    std::cout<<"hash by string="<<ag.toString()<<std::endl;
//    std::cout<<"hash by uint_64="<<ag.hash()<<std::endl;
//    ag.fromHash(ag.hash(),4);
//    // 100
//    // 001 000 000
//    exit(-1);
    auto &IC = IsomerCounter::GetInstance();
    IC.calculate(32, "D:\\TEMP\\alkane");
    return 0;

}
