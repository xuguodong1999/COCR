#include "ui/main_tab_widget.h"
#include "application.hpp"
#include "chem/jmol_adapter.hpp"
#include <iostream>
#include <QDebug>

#include "ocr/ocr_runnable.hpp"
#include <QDir>

// TODO: 不考虑子图、添加 NH 支持
int loopBenchMark() {
    QString imgRoot, refRoot;
#ifdef Q_OS_LINUX
    imgRoot = "/home/xgd/source/repos/leafxy/testcase/public/";
#elif defined(Q_OS_WIN)
    imgRoot = "D:/OCSR_Review/assets/images/";
    refRoot = "D:/OCSR_Review/assets/reference/";
#endif
    auto clef = QDir(imgRoot + "CLEF").entryInfoList(QDir::Filter(QDir::Files));
    auto jpo = QDir(imgRoot + "JPO").entryInfoList(QDir::Filter(QDir::Files));
    auto uob = QDir(imgRoot + "UOB").entryInfoList(QDir::Filter(QDir::Files));
    auto uspto = QDir(imgRoot + "USPTO").entryInfoList(QDir::Filter(QDir::Files));
    OCRThread ocrThread;
    int sampleNum = 0;
    enum BenchmarkState {
        INCHI_OK = 0,
        IMAGE_FILE_LOAD_FAILED = 1,
        REF_FILE_LOAD_FAILED = 2,
        REF_MOL_LOAD_FAILED = 3,

        MOL_OCR_FAILED = 4,
        MOL_PROCESS_FAILED = 5,
        MOL_COMPARE_FAILED = 6,
    };
    std::unordered_map<BenchmarkState, int> stateMap = {
            {INCHI_OK,               0},
            {IMAGE_FILE_LOAD_FAILED, 0},
            {REF_FILE_LOAD_FAILED,   0},
            {REF_FILE_LOAD_FAILED,   0},
            {MOL_OCR_FAILED,         0},
            {MOL_PROCESS_FAILED,     0},
            {MOL_COMPARE_FAILED,     0},
    };
    QString imagePath = "D:/leafxy_benchmark/image";
    QString refPath = "D:/leafxy_benchmark/ref";
    std::vector<QString> subDirName = {"CLEF", "JPO", "UOB", "USPTO"};
    std::unordered_map<QString, QString> imageSuffixMap = {
            {"CLEF",  "png"},
            {"JPO",   "png"},
            {"UOB",   "png"},
            {"USPTO", "png"},
    };
    std::unordered_map<QString, QString> refSuffixMap = {
            {"CLEF",  "mol"},
            {"JPO",   "sdf"},
            {"UOB",   "mol"},
            {"USPTO", "mol"},
    };
    auto fail_and_exit = [&](const BenchmarkState &state, const QString &message) {
        ++stateMap[state];
        qDebug() << "exit for" << state;
        qDebug() << message.toStdString().c_str();
        exit(-1);
    };
    auto fail_and_continue = [&](const BenchmarkState &state, const QString &message) {
        ++stateMap[state];
        qDebug() << "fail for" << state;
        qDebug() << message.toStdString().c_str();
    };
    auto handle_image = [&](const QString &fileName,
                            const QString &subDirName = "CLEF") -> void {
        sampleNum++;
        QString imageFilePath = imagePath + "/" + subDirName + "/" + fileName;
        QString refFilePath = refPath + "/" + subDirName + "/" + QString(fileName).replace(
                "." + imageSuffixMap[subDirName],
                "." + refSuffixMap[subDirName]);
        QImage image;
        if (!image.load(imageFilePath)) {
            fail_and_exit(IMAGE_FILE_LOAD_FAILED, imageFilePath);
        }
        qDebug() << imageFilePath.toStdString().c_str();
        image = image.convertToFormat(QImage::Format_Grayscale8);
        QFile f(refFilePath);
        if (!f.open(QIODevice::ReadOnly)) {
            fail_and_exit(REF_FILE_LOAD_FAILED, refFilePath);
        }
        std::string refBuffer = f.readAll().toStdString();
        f.close();
        std::shared_ptr<xgd::JMol> refMol;
        try {
            refMol = std::make_shared<xgd::JMolAdapter>();
            refMol->readAs(refBuffer, refSuffixMap[subDirName].toStdString());
        } catch (std::exception &e) {
            fail_and_exit(REF_MOL_LOAD_FAILED, e.what());
        }
        try {
            ocrThread.bindData(image);
            ocrThread.start();
            ocrThread.wait();
        } catch (std::exception &e) {
            fail_and_continue(MOL_OCR_FAILED, e.what());
            return;
        }
        auto mol = ocrThread.getMol();
        if (!mol) {
            fail_and_continue(MOL_OCR_FAILED, "mol == nullptr");
            return;
        }
        try {
            if (!mol->tryExpand()) {
                fail_and_continue(MOL_PROCESS_FAILED, "expand");
                return;
            }
            mol = mol->deepClone();
            mol->addAllHydrogens();
        } catch (std::exception &e) {
            fail_and_continue(MOL_PROCESS_FAILED, e.what());
            return;
        }
        auto smi0 = mol->writeAs("can");
        auto smi1 = refMol->writeAs("can");
        if (smi0 == smi1) {
            ++stateMap[INCHI_OK];
            qDebug() << stateMap[INCHI_OK] / (float) sampleNum;
            return;
        }
        QString debug = QString::fromStdString(smi0) + " **vs**" + smi1.c_str();
        debug.replace("\r", "\n");
        debug.replace("\t", "\n");
        debug.replace("\n\n", "\n");
        fail_and_continue(MOL_COMPARE_FAILED, debug);
    };
    auto loop_dataset = [&](const QString &subDirName) {
        auto files = QDir(imagePath + "/" + subDirName).entryInfoList(
                QDir::Filter(QDir::Files));
        for (auto &file:files) {
            handle_image(file.fileName(), subDirName);
        }
        qDebug() << "********" << subDirName << "******** DONE ********";
    };
//    loop_dataset("UOB");
    loop_dataset("USPTO");
    qDebug() << stateMap[INCHI_OK] / (float) sampleNum;
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

#include <QFile>
#include "hw/hw_mol.hpp"

int main(int argc, char *argv[]) {
//    QFile f("C:/source/repos/leafxy/resources/drugbank.smi");
//    f.open(QIODevice::ReadOnly);
//    while (!f.atEnd()) {
//        QString s = f.readLine();
//        if (s.contains(".") || s.contains("/") || s.contains("\\")
//            || s.contains("[") || s.contains("@") || s.contains("P") || s.contains("3") || s.length() > 30) {
//            continue;
//        }
//        auto mol = std::make_shared<xgd::JMolAdapter>();
//        mol->readAsSMI(s.toStdString());
//        auto hwMol = std::make_shared<HwMol>(mol);
//        hwMol->showOnScreen(1);
//        qDebug() << mol->writeAs("can").c_str();
//    }
//    f.close();
//    return 0;
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
//    return loopBenchMarkWrapper();
    Application app(argc, argv);
    MainTabWidget w;
    w.show();
    return app.exec();
}
