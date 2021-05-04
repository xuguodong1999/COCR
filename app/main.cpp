#include "ui/main_tab_widget.h"
#include "application.hpp"
#include "chem/jmol_adapter.hpp"
#include <iostream>
#include <QDebug>

#include "ocr/ocr_runnable.hpp"
#include <QDir>

// TODO: 不考虑子图、添加 NH 支持
int loopBenchMark() {
#ifdef Q_OS_LINUX
    QString imgRoot("/home/xgd/source/repos/leafxy/testcase/public/");
#elif defined(Q_OS_WIN)
    QString imgRoot("D:/OCSR_Review/assets/images/");
    QString refRoot("D:/OCSR_Review/assets/reference/");
#endif
    auto clef = QDir(imgRoot + "CLEF").entryInfoList(QDir::Filter(QDir::Files));
    auto jpo = QDir(imgRoot + "JPO").entryInfoList(QDir::Filter(QDir::Files));
    auto uob = QDir(imgRoot + "UOB").entryInfoList(QDir::Filter(QDir::Files));
    auto uspto = QDir(imgRoot + "USPTO").entryInfoList(QDir::Filter(QDir::Files));
    OCRThread ocrThread;
    int sampleNum, dir4Expand, dir4ImageLoad, dir4Final, dir4TruthFileOpen, die4Exception;
    sampleNum = dir4Expand = dir4ImageLoad = dir4Final = dir4TruthFileOpen = die4Exception = 0;
    auto getDieNum = [&]() -> int {
        return dir4Expand + dir4ImageLoad + dir4Final + dir4TruthFileOpen + die4Exception;
    };
    auto logDie = [&]() -> void {
        qDebug() << "SampleNum";
        qDebug() << sampleNum;
        qDebug() << "Expand" << "ImageLoad" << "Final" << "TruthFileOpen" << "Exception";
        qDebug() << dir4Expand << dir4ImageLoad << dir4Final << dir4TruthFileOpen << die4Exception;
    };
    auto loop = [&](const QString &_filePath) {
        QImage image;
        if (image.load(_filePath)) {
            ++sampleNum;
            image = image.convertToFormat(QImage::Format_Grayscale8);
            ocrThread.bindData(image);
            ocrThread.start();
            ocrThread.wait();
            auto mol = ocrThread.getMol();
            if (mol) {
                if (!mol->tryExpand()) {
                    ++dir4Expand;
                    return;
                }
                try {
                    mol->addAllHydrogens();
                    auto predict = mol->writeAs("inchi");
//                    QFile f(QString(_filePath).replace("images/USPTO", "reference/USPTO_mol_ref"
//                    QFile f(QString(_filePath).replace("images/CLEF", "reference/CLEF_mol_ref"
                    QFile f(QString(_filePath).replace("images/JPO", "reference/JPO_mol_ref"
                    ).replace("png", "sdf"));
                    if (!f.open(QIODevice::ReadOnly)) {
                        ++dir4TruthFileOpen;
                        qDebug() << "[die] file open:" << f.fileName();
                    }
                    auto buffer = f.readAll().toStdString();
                    f.close();
                    auto trueMol = std::make_shared<xgd::JMolAdapter>();
                    trueMol->readAs(buffer, "mol");
                    auto truth = trueMol->writeAs("inchi");
                    if (predict != truth) {
                        ++dir4Final;
                        qDebug() << "[die] compare:";
                        qDebug() << "file=" << _filePath;
                        qDebug() << "smi0=" << mol->writeAsSMI().c_str();
                        qDebug() << "smi1=" << trueMol->writeAsSMI().c_str();
                        qDebug() << "inchi0=" << predict.c_str();
                        qDebug() << "inchi1=" << truth.c_str();
                    } else {
                        qDebug() << "[haha] acc=" << 1.0 * (sampleNum - getDieNum()) / sampleNum;
                    }
                } catch (std::exception &e) {
                    ++die4Exception;
                    qDebug() << "[die] exception:" << e.what();
                }
            }
        } else {
            qDebug() << "[die] image open:" << _filePath;
            ++dir4ImageLoad;
        }
    };
//    for (auto &file:clef) { loop(file.absoluteFilePath()); }
//    qDebug() << "CLEF DONE **************";
    for (auto &file:jpo) { loop(file.absoluteFilePath()); }
    qDebug() << "JPO DONE **************";
//    for (auto &file:uob) { loop(file.absoluteFilePath()); }
//    qDebug() << "UOB DONE **************";
//    for (auto &file:uspto) { loop(file.absoluteFilePath()); }
//    qDebug() << "USPTO DONE **************";
    logDie();
    return 0;
}

int loopBenchMarkWrapper() {
    try {
        return loopBenchMark();
    } catch (std::exception &e) {
        qDebug() << e.what();
        return -1;
    }
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
//    return loopBenchMarkWrapper();
    Application app(argc, argv);
    MainTabWidget w;
    w.show();
    return app.exec();
}
