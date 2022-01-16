#include "config.hpp"
#include "crnn_data.hpp"
#include "soso_darknet.hpp"
#include "hw_data.hpp"
#include <QCoreApplication>

#include <random>
#include <memory>
#include <iostream>
#include <QApplication>

extern CRNNDataGenerator crnnDataGenerator;

void generateCRNNData() {
    srand(0633);
    crnnDataGenerator.init("/media/xgd/hjyy-ext4/soso-text");
    crnnDataGenerator.dump(2000000, 50000);
}

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    HwDataLoader::getInstance();
    try {
        generateCRNNData();
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
