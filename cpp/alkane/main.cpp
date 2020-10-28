#include "config.hpp"
#include "generator.hpp"
#include "isomer.hpp"
#include "mol.hpp"
#include <iostream>

int main(int argc, char **argv) {
    auto alkane_dir = getAlkaneCacheDir();
//    // 所有异构体的结构生成
//    auto &IC = IsomerCounter::GetInstance();
//    // 1-27碳，总时间 ~ 4h，
//    IC.calculate(27, alkane_dir.c_str());
//    // 多线程方案：28碳，小新pro13，i7，kubuntu，4h，恰好占满15G内存
//    IC.calculate_i_from_i_1(alkane_dir.c_str(), 28);
    // 读取烷烃异构体并生成SMILES串、添加杂原子和基团修饰
//    auto &sg = SMILESGenerator::GetInstance();
//    for (int i = 12; i <= 12; i++) {
//        sg.HashToRichSMILES(alkane_dir.c_str(), i);
//    }
    // 测试类型转换
    std::string testSample;
    while(std::cin>>testSample){
        try {
            JMol jMol(testSample);
            jMol.update2DCoordinates();
        } catch (std::exception e) {
            std::cerr<<e.what()<<std::endl;
        }
    }
    return 0;

}
