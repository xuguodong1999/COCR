#include "crnn_data.hpp"
#include "soso_darknet.hpp"
#include <QCoreApplication>

#include <random>
#include <memory>
#include <iostream>

extern std::string WORKSPACE;

void generateCRNNData(){
    srand(0633);
    CRNNDataGenerator generator;
    generator.init(WORKSPACE + "soso-text");
    generator.dump(2000000, 10000);
//        generator.display();
}
void generateYoloData(){
    srand(0022);
    SOSODarknet generator;
    generator.init(WORKSPACE+"soso-obj");
    generator.dump(2000000,2);
}
int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    try {
        generateCRNNData();
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
