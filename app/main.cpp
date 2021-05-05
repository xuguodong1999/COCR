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
    QString referencePath = "D:/leafxy_benchmark/ref";
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
        qDebug() << message;
        exit(-1);
    };
    auto fail_and_continue = [&](const BenchmarkState &state, const QString &message) {
        ++stateMap[state];
        qDebug() << "fail for" << state;
        qDebug() << message;
    };
    auto handle_image = [&](const QString &fileName,
                            const QString &subDirName = "CLEF") -> void {
        QString imageFilePath = imagePath + "/" + subDirName + "/" + fileName;
        QString refFilePath = imageFilePath.replace(
                "." + imageSuffixMap[subDirName],
                "." + refSuffixMap[subDirName]);
        QImage image;
        if (!image.load(imageFilePath)) {
            fail_and_exit(IMAGE_FILE_LOAD_FAILED, imageFilePath);
        }
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
        ++sampleNum;
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
            mol->addAllHydrogens();
        } catch (std::exception &e) {
            fail_and_continue(MOL_PROCESS_FAILED, e.what());
            return;
        }
        auto inchi0 = mol->writeAs("inchi");
        auto inchi1 = refMol->writeAs("inchi");
        if (inchi0 == inchi1) {
            ++stateMap[INCHI_OK];
            return;
        }
        auto smi0 = mol->writeAsSMI();
        auto smi1 = refMol->writeAsSMI();
        QString debug = QString::fromStdString(smi0) + smi1.c_str();
        debug.replace("\r", "\n");
        debug.replace("\t", "\n");
        debug.replace("\n\n", "\n");
        fail_and_continue(MOL_COMPARE_FAILED, debug);
    };
    auto loop_dataset = [&](const QString &subDirName) {
        auto files = QDir(imgRoot + "/" + subDirName).entryInfoList(
                QDir::Filter(QDir::Files));
        for (auto &file:files) {
            handle_image(file.fileName(), subDirName);
        }
        qDebug() << "********" << subDirName << "******** DONE ********";
    };
    loop_dataset("JPO");
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
