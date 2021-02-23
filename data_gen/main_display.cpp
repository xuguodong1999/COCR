#include "crnn_data.hpp"
#include "soso_darknet.hpp"
#include "hw_data.hpp"
#include <QCoreApplication>

#include <random>
#include <memory>
#include <iostream>

extern std::string WORKSPACE;

CRNNDataGenerator crnnDataGenerator;


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
        testYoloDara();
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
