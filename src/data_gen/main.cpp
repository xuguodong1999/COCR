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
#include <QDebug>
#include <QDir>

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

static QString USAGE_MEG = "";

int main(int argc, char **argv) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication a(argc, argv);
    auto arguments = a.arguments();
    if (arguments.empty()) { // display something
        HwDataLoader::getInstance();
        displayCRNNData();
    } else if (arguments.size() == 3) {
        auto dataType = arguments.at(0).toLower();
        auto sampleNum = arguments.at(1);
        auto outputDir = arguments.at(2);
        bool ok = false;
        int sample_num = sampleNum.toLong(&ok, 10);
        if (!ok) {
            qDebug() << sampleNum << " is not a number";
            qDebug() << USAGE_MEG;
        } else if (!QDir(outputDir).exists()) {
            qDebug() << outputDir << " is not an existing directory";
            qDebug() << USAGE_MEG;
        } else if ("-yolo" == dataType) { // generate training data for yolo

        } else if ("-crnn" == dataType) { // generate training data for crnn

        } else if("-isomer" == dataType) { // generate isomer data for happy

        }else {
            qDebug() << USAGE_MEG;
        }
    } else {
        qDebug() << USAGE_MEG;
    }
    return 0;
}