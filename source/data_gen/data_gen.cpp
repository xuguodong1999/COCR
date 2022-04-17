#include "data/soso_crnn.h"
#include "data/soso_obj.h"
#include "math/isomer.h"
#include "math/polya.h"
#include "stroke/couch_sym.h"

#include <QCoreApplication>
#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QLabel>
#include <QApplication>
#include <QDebug>
#include <QDir>

#include <iostream>

void displayCRNNData() {
    srand(42);
    crnnDataGenerator.initData();
    crnnDataGenerator.display();
}

void displayYoloData() {
    srand(42);
    crnnDataGenerator.initData();
    SOSODarknet generator;
    generator.display();
}

void generateCRNNData(const std::string &dir, const size_t &num) {
    srand(42);
    if (!crnnDataGenerator.init(dir)) {
        std::cerr << __FUNCTION__ << " failed by CRNNDataGenerator.init" << std::flush;
        exit(-1);
    }
    crnnDataGenerator.dump(num, 0);
}

void generateYoloData(const std::string &dir, const size_t &num) {
    srand(42);
    crnnDataGenerator.initData();
    SOSODarknet generator;
    if (!generator.init(dir)) {
        std::cerr << __FUNCTION__ << " failed by SOSODarknet.init" << std::flush;
        exit(-1);
    }
    generator.dump(num, 1);
}

void calcIsomers(const std::string &dir, const size_t &carbonNum) {
    auto &p = PolyaIsomerCounter::GetInstance();
    p.count(30);
    auto &isomer = IsomerCounter::GetInstance();
    isomer.calculate(carbonNum, dir.c_str());
}

static const char *USAGE_MEG = "data_gen usage:\n"
                               "(1)\t./data_gen\n"
                               "(2)\t./data_gen -yolo [number of samples] [an empty, existing directory path]\n"
                               "(3)\t./data_gen -crnn [number of samples] [an empty, existing directory path]\n"
                               "(4)\t./data_gen -isomer [number of samples] [an empty, existing directory path]\n";

int main(int argc, char **argv) {
//#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
//    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
//#endif
    QApplication a(argc, argv);
    auto arguments = a.arguments();
    if (arguments.size() == 1) { // display something
        HwDataLoader::getInstance();
//        displayYoloData();
        displayCRNNData();
        return a.exec();
    } else if (arguments.size() == 4) {
        const auto dataType = arguments.at(1).toLower();
        const auto &sampleNum = arguments.at(2);
        const auto &outputDir = arguments.at(3);
        bool ok = false;
        long sample_num = sampleNum.toLong(&ok, 10);
        QDir outDir(outputDir);
        if (!ok) {
            qDebug() << sampleNum << " is not a number";
            std::cerr << USAGE_MEG << std::flush;
        } else if (!outDir.exists()) {
            qDebug() << outputDir << " is not an existing directory";
            std::cerr << USAGE_MEG << std::flush;
        } else if ("-yolo" == dataType) { // generate training data for yolo
            generateYoloData(outDir.absolutePath().toStdString() + "/yolo_data/", sample_num);
        } else if ("-crnn" == dataType) { // generate training data for crnn
            generateCRNNData(outDir.absolutePath().toStdString() + "/crnn_data/", sample_num);
        } else if ("-isomer" == dataType) { // generate isomer data for happy
            calcIsomers(outDir.absolutePath().toStdString(), sample_num);
        } else {
            std::cerr << USAGE_MEG << std::flush;
        }
    } else {
        std::cerr << USAGE_MEG << std::flush;
    }
    return 0;
}