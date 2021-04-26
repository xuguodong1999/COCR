#include "ui/main_tab_widget.h"
#include "application.hpp"
#include "chem/jmol.hpp"
#include "chem/jmol_adapter.hpp"
#include <iostream>

int testMolFormat() {
    std::vector<std::string> formats = GetWritableFormats();
    std::vector<std::string> suffixVec;
    for (auto &format:formats) {
        if (std::string::npos != format.find("Read-only")) { continue; }
        suffixVec.push_back(format.substr(0, format.find(" ")));
        std::cout << "\"" << suffixVec.back() << "\",";
    }
    std::cout << std::endl;
    std::shared_ptr<xgd::JMol> mol = std::make_shared<xgd::JMolAdapter>();
    mol->readAsSMI("c1ccccc1");
    std::cout << "**********************" << std::endl;
    for (auto &format:suffixVec) {
        std::cout << format << std::endl;
        std::string content = mol->writeAs(format);
        if (content.length() > 10000) { content = "error"; }
        std::cout << content << std::endl;
        std::cout << "**********************" << std::endl;
    }
    return 0;
}

#include <QDebug>

int testReg() {
    QString fuck = "CCl3";
    QRegExp reg("[CONPS][(Br)(Cl)(F)(I)]");
    qDebug() << fuck;
    qDebug() << fuck.replace(reg, ".");
    return 0;
}

#include "ocr/ocr_runnable.hpp"
#include <QDir>

int loopBenchMark() {
#ifdef Q_OS_LINUX
    QString root("/home/xgd/source/repos/leafxy/testcase/public/");
#elif defined(Q_OS_WIN)
    QString root("C:/Users/xgd/source/repos/leafxy/testcase/public/");
#endif
    auto clef = QDir(root + "CLEF").entryInfoList(QDir::Filter(QDir::Files));
    auto jpo = QDir(root + "JPO").entryInfoList(QDir::Filter(QDir::Files));
    auto uob = QDir(root + "UOB").entryInfoList(QDir::Filter(QDir::Files));
    auto uspto = QDir(root + "USPTO").entryInfoList(QDir::Filter(QDir::Files));
    OCRThread ocrThread;
    auto loop = [&](const QString &_filePath) {
        QImage image;
        if (image.load(_filePath)) {
            image = image.convertToFormat(QImage::Format_Grayscale8);
            ocrThread.bindData(image);
            ocrThread.start();
            ocrThread.wait();
            auto mol = ocrThread.getMol();
            if (mol) {
                if (!mol->tryExpand()) {

                }
                mol->addAllHydrogens();
            }
        }
    };
    for (auto &file:clef) { loop(file.absoluteFilePath()); }
    qDebug() << "CLEF DONE **************";
    for (auto &file:jpo) { loop(file.absoluteFilePath()); }
    qDebug() << "JPO DONE **************";
    for (auto &file:uob) { loop(file.absoluteFilePath()); }
    qDebug() << "UOB DONE **************";
    for (auto &file:uspto) { loop(file.absoluteFilePath()); }
    qDebug() << "USPTO DONE **************";
    return 0;
}

/**
 * FIXME: 结构综合失败的 case 总结：
 * 【不解 bug】超原子一段有键，没有生成这个键
 * 【不解 bug】无意义超原子干扰视线
 * 【必解 bug】超原子多端接入
 * 【必解 bug】上下邻接的单原子图元
 * 【必解 bug】两个键连原子靠得近
 */
int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
//    return testMolFormat();
//    return testReg();
//    return loopBenchMark();
    Application app(argc, argv);
    MainTabWidget w;
    w.show();
    return app.exec();
}
