#include "crnn_data.hpp"
#include "soso_darknet.hpp"
#include "hw_data.hpp"
#include "isomer.hpp"
#include "polya.hpp"

#include <QCoreApplication>
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QLabel>
#include <QApplication>

#include <opencv2/highgui.hpp>

#include <memory>
#include <iostream>

extern CRNNDataGenerator crnnDataGenerator;

void displayCRNNData() {
    srand(42);
    crnnDataGenerator.init("path/to/somewhere1");
    crnnDataGenerator.display();
}

void generateCRNNData() {
    srand(42);
    crnnDataGenerator.init("/path/to/somewhere1");
    crnnDataGenerator.dump(2000000, 50000);
}

void displayYoloData() {
    SOSODarknet generator;
    generator.init("path/to/somewhere2");
    generator.display();
    generator.dump(1000000, 5);
}

void genYoloData() {
    srand(42);
    crnnDataGenerator.init("/path/to/somewhere1");
    SOSODarknet generator;
    generator.init("/path/to/somewhere2");
    generator.dump(200000, 5);
}

void calcIsomers() {
    auto &isomer = IsomerCounter::GetInstance();
    isomer.calculate_i_from_i_1("/path/to/a/large/disk", 30);
}

void checkIsomers() {
    auto &p = PolyaIsomerCounter::GetInstance();
    p.count(30);
}

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    HwDataLoader::getInstance();
    try {
        displayCRNNData();
        return a.exec();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
