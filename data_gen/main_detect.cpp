#include "crnn_data.hpp"
#include "soso_darknet.hpp"
#include "hw_data.hpp"
#include <QCoreApplication>

#include <random>
#include <memory>
#include <iostream>

extern std::string WORKSPACE;

CRNNDataGenerator crnnDataGenerator;

void generateCRNNData() {
    srand(0633);
//    generator.init(WORKSPACE + "soso-text-nb");
    crnnDataGenerator.init("/media/xgd/hjyy-ext4/soso-text");
    crnnDataGenerator.dump(2000000, 10000);
//        generator.display();
}

void generateYoloData() {
    srand(0022);
    SOSODarknet generator;
    generator.init("/home/xgd/datasets/soso-obj");
    generator.dump(2000000, 10);
}

void testYoloDara() {
    srand(0022);
    crnnDataGenerator.init("/tmp/fuck");
    SOSODarknet generator;
    generator.init(WORKSPACE + "soso-obj");
    generator.display();
}

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    HwDataLoader::getInstance();
    try {
//        generateCRNNData();
        testYoloDara();
//        generateYoloData();
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
