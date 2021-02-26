#include "crnn_data.hpp"
#include "soso_darknet.hpp"
#include "hw_data.hpp"
#include <QCoreApplication>

#include <random>
#include <memory>
#include <iostream>
#include <QApplication>

extern std::string WORKSPACE;

CRNNDataGenerator crnnDataGenerator;


void generateYoloData() {
    srand(0022);
    crnnDataGenerator.init("/tmp/fuck");
    SOSODarknet generator;
//    generator.init(WORKSPACE + "soso-obj");
    generator.init("/home/xgd/datasets/soso-obj");
    generator.dump(200000, 5);
}

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    HwDataLoader::getInstance();
    try {
        generateYoloData();
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
