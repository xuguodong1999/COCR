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
#ifdef Q_OS_WIN
    QString imagePath = "D:/leafxy_benchmark/image";
    QString refPath = "D:/leafxy_benchmark/ref";
#else
    QString imagePath = "/media/xgd/数据盘/leafxy_benchmark/image";
    QString refPath = "/media/xgd/数据盘/leafxy_benchmark/ref";
#endif
    std::vector <QString> subDirName = {"CLEF", "JPO", "UOB", "USPTO"};
    std::unordered_map <QString, QString> imageSuffixMap = {
            {"CLEF",  "png"},
            {"JPO",   "png"},
            {"UOB",   "png"},
            {"USPTO", "png"},
    };
    std::unordered_map <QString, QString> refSuffixMap = {
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
        std::shared_ptr <xgd::JMol> refMol;
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
    loop_dataset("UOB");
//    loop_dataset("JPO");
//    loop_dataset("USPTO");
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
#include <opencv2/highgui.hpp>
#include "hw/hw_mol.hpp"
#include "opencv_util.hpp"
#include <random>
#include <QJsonDocument>
#include <QJsonObject>

int washDrugbank() {
    //    QFile f("C:/source/repos/leafxy/resources/drugbank.smi");
    QFile f("C:/Users/xgd/source/repos/leafxy/resources/drugbank_simple.smi");
//    QFile fout("C:/Users/xgd/source/repos/leafxy/resources/drugbank_simple.smi");
//    fout.open(QIODevice::WriteOnly);
    f.open(QIODevice::ReadOnly);
    OCRThread ocrThread;
    float success = 0, all = 0, full = 0;
    while (!f.atEnd()) {
        full += 1;
        QString s = f.readLine().trimmed();
        if (s.contains(".") || s.contains("/") || s.contains("\\")
            || s.contains("[") || s.contains("@") || s.contains("3")) {
            continue;
        }
        auto refMol = std::make_shared<xgd::JMolAdapter>();
        refMol->readAsSMI(s.toStdString());
        if (refMol->getBondNum() < 1) { continue; }
        auto refSMI = refMol->writeAs("can");
        qDebug() << refSMI.c_str();
//        refMol->readAsSMI(refSMI);
        auto hwMol = std::make_shared<HwMol>(refMol);
        auto image = hwMol->showOnScreen(1, true);
        try {
            ocrThread.bindData(image[0].first);
            ocrThread.start();
            ocrThread.wait();
        } catch (...) {
            continue;
        }
        auto mol = ocrThread.getMol();
        if (mol) {
            all += 1;
            try {
                mol->tryExpand();
                mol->addAllHydrogens();
            } catch (...) {
                continue;
            }
            auto smi = mol->writeAsSMI();
            auto ss = QString::fromStdString(smi);
            if (ss.contains("@") || ss.contains("/") || ss.contains("\\") || ss.contains("At")) {
                continue;
            }
            if (smi == refSMI) {
                success += 1;
            } else {
                qDebug() << "ref=" << refSMI.c_str();
                qDebug() << "smi=" << smi.c_str();
            }
//            fout.write(s.toLocal8Bit() + "\n");
            qDebug() << "acc=" << success / all << ",all=" << all << ",success=" << success << ",full=" << full;
//            cv::waitKey(0);
        }
    }
    f.close();
//    fout.close();
    return 0;
}


int benchmarkDrugbank() {
    QString E2E_DIR = "D:/Gitlab-leafxy/e2e/";
//    QFile f("C:/Users/xgd/source/repos/leafxy/resources/drugbank_simple.smi");
    QFile f("C:/source/repos/leafxy/resources/drugbank_simple.smi");
    f.open(QIODevice::ReadOnly);
//    QFile tFile("D:/Gitlab-leafxy/e2e/data.txt");
//    tFile.open(QIODevice::WriteOnly);
    QJsonDocument d;
    QJsonArray arr;
    OCRThread ocrThread;
    float success, all, die_ocr, die_empty_mol;
    success = all = die_empty_mol = die_ocr = 0;
    std::vector <QString> smiles;
    while (!f.atEnd()) {
        smiles.push_back(f.readLine().trimmed());
    }
    f.close();
//    std::shuffle(smiles.begin(), smiles.end(), std::default_random_engine());
    float sub_all = 0, sub_ok = 0;
    int idx = 0;
    for (auto &smi:smiles) {
        sub_all += 1;
//        if (sub_all >= 20) { break; }
        auto refMol = std::make_shared<xgd::JMolAdapter>();
        refMol->readAsSMI(smi.toStdString());
        std::string refCan = refMol->writeAs("can");
        auto hwMol = std::make_shared<HwMol>(refMol);
        auto images = hwMol->showOnScreen(10, false);
        float sub_success = 0;
        std::vector <size_t> okVec, dieVec;
        for (size_t i = 0; i < images.size(); i++) {
            auto &[image, jsonObj] = images[i];
            try {
                all += 1;
                cv::Mat tmp = image.clone();
                ocrThread.bindData(tmp);
                ocrThread.start();
                ocrThread.wait();
            } catch (...) {
                die_ocr += 1;
                dieVec.push_back(i);
                continue;
            }
            auto mol = ocrThread.getMol();
            if (!mol) {
                die_empty_mol += 1;
                dieVec.push_back(i);
                continue;
            }
            try {
                if (!mol->tryExpand()) {
                    dieVec.push_back(i);
                    continue;
                }
                mol->addAllHydrogens();
            } catch (...) {
                dieVec.push_back(i);
                continue;
            }
            auto can = mol->writeAsSMI();
            if (can == refCan) {
                success += 1;
                sub_success += 1;
                okVec.push_back(i);
            } else {
                dieVec.push_back(i);
                qDebug() << "ref=" << refCan.c_str();
                qDebug() << "smi=" << can.c_str();
            }
            qDebug() << "acc=" << success / all <<
                     ",all=" << all <<
                     ",success=" << success <<
                     ",die_ocr=" << die_ocr <<
                     ",die_empty_mol=" << die_empty_mol;
        }
        if (sub_success >= 5) {
            sub_ok += 1;
            qDebug() << "sub_acc=" << sub_ok / sub_all << ",sub_all=" << sub_all;
        }
//        tFile.write(("smi:" + QString::fromStdString(refCan).trimmed() + "\n").toLocal8Bit());
        int num = 0;
        for (size_t j = 0; j < okVec.size(); j++) {
            if (num == 5) { break; }
            std::string filename = QString::number(idx++, 36).toStdString();
            cv::imwrite(E2E_DIR.toStdString() + "JPEGImages/" + filename + ".jpg", images[okVec[j]].first,
                        {cv::IMWRITE_JPEG_QUALITY, 50});
            auto &obj = images[okVec[j]].second;
            obj.insert("smi", QString::fromStdString(refCan).trimmed());
            obj.insert("image", "" + QString::fromStdString(filename + ".jpg"));
            arr.push_back(std::move(obj));
            ++num;
        }
        for (size_t j = 0; j < dieVec.size(); j++) {
            if (num == 5) { break; }
            std::string filename = QString::number(idx++, 36).toStdString();
            cv::imwrite(E2E_DIR.toStdString() + "JPEGImages/" + filename + ".jpg", images[dieVec[j]].first,
                        {cv::IMWRITE_JPEG_QUALITY, 50});
            auto &obj = images[dieVec[j]].second;
            obj.insert("smi", QString::fromStdString(refCan).trimmed());
            obj.insert("image", "" + QString::fromStdString(filename + ".jpg"));
            arr.push_back(std::move(obj));
            ++num;
        }
        qDebug() << "sub_success=" << sub_success;
    }
    d.setArray(arr);
    QFile fuck(E2E_DIR + "data.json");
    fuck.open(QIODevice::WriteOnly);
    fuck.write(d.toJson(QJsonDocument::Compact));
    fuck.close();
    return 0;
}

#include "hw/timer.hpp"
#include <rapidjson/document.h>

int stateE2E() {
    QString E2E_DIR = "D:/Gitlab-leafxy/e2e/";
    QFile f(E2E_DIR + "data.json");
    f.open(QIODevice::ReadOnly);
    auto buf = f.readAll();
    f.close();
    rapidjson::Document d;
    d.Parse(buf.data());
    auto arr = d.GetArray();
    std::unordered_map<std::string, int> eleCounter;
    std::vector<int> bondCounter = {0, 0, 0, 0, 0};
    int countC = 0;
    for (auto val = arr.begin(); val != arr.end(); val++) {
        auto obj = val->GetObject();
        auto groundTruth = obj["smi"].GetString();
        auto mol = std::make_shared<xgd::JMolAdapter>();
        mol->readAsSMI(groundTruth);
        mol->loopAtomVec([&](xgd::JAtom &atom) {
            auto it = eleCounter.find(atom.getName());
            if (eleCounter.end() == it) {
                eleCounter[atom.getName()] = 1;
            } else {
                ++it->second;
            }
        });
        mol->loopBondVec([&](xgd::JBond &bond) {
            ++bondCounter[bond.getBondOrder()];
        });
        auto atoms = obj["atoms"].GetArray();
        for (auto atom = atoms.begin(); atom != atoms.end(); atom++) {
            auto obj = atom->GetObject();
            if (obj["element"].GetString() == std::string("C") && obj["h"].GetDouble() > 0.0001 &&
                obj["w"].GetDouble() > 0.0001) {
                ++countC;
            }
        }
    }
    std::cout << "cc=" << countC << std::endl;
    for (auto &c:bondCounter) {
        std::cout << c << ",";
    }
    std::cout << std::endl;
    for (auto&[name, num]:eleCounter) {
        std::cout << name << "," << num << std::endl;
    }
    return 0;
}

int benchmarkE2E() {
    QString E2E_DIR = "D:/Gitlab-leafxy/e2e/";
    QFile f(E2E_DIR + "data.json");
    f.open(QIODevice::ReadOnly);
    auto buf = f.readAll();
    f.close();
    rapidjson::Document d;
    d.Parse(buf.data());
    auto arr = d.GetArray();
    OCRThread ocrThread;
    float sample_num(0), correct_num(0);
    Timer timer;
    timer.start(true);
    std::vector<float> times, spaces;
    std::vector <std::string> failedCases;
//    qDebug() << "arr.size=" << arr.size();
    for (auto val = arr.begin(); val != arr.end(); val++) {
        auto obj = val->GetObject();
        auto groundTruth = obj["smi"].GetString();
        auto file = obj["image"].GetString();
        std::string filepath = (E2E_DIR + "JPEGImages/" + file).toStdString();
        try {
            cv::Mat image = cv::imread(filepath);
            spaces.push_back(std::sqrt(image.rows * image.cols));
            sample_num += 1;
            ocrThread.bindData(image);
            ocrThread.start();
            ocrThread.wait();
        } catch (...) {
            times.push_back(timer.display_duration());
            failedCases.push_back(file);
            continue;
        }
        auto mol = ocrThread.getMol();
        if (!mol) {
            times.push_back(timer.display_duration());
            failedCases.push_back(file);
            continue;
        }
        try {
            mol->tryExpand();
            mol->addAllHydrogens();
        } catch (...) {
            times.push_back(timer.display_duration());
            failedCases.push_back(file);
            continue;
        }
        auto smi = mol->writeAsSMI();
        if (smi == groundTruth) {
            correct_num += 1;
            times.push_back(timer.display_duration());
            std::cout << "N=" << sample_num << ",T=" << correct_num << ",acc=" << correct_num / sample_num << "\n";
        } else {
            times.push_back(timer.display_duration());
            failedCases.push_back(file);
        }
    }
    timer.stop(true);
    f.close();
    std::cout << std::endl << "times=[";
    for (auto &time:times) {
        std::cout << time << ",";
    }
    std::cout << "]" << std::endl;
    std::cout << std::endl << "spaces=[";
    for (auto &space:spaces) {
        std::cout << space << ",";
    }
    std::cout << "]" << std::endl;
    std::cout << std::endl << "failed cases=[";
    for (auto &caseName:failedCases) {
        std::cout << "\"" << caseName << "\",";
    }
    std::cout << "]" << std::endl;
    return 0;
}

int main(int argc, char *argv[]) {
//    return stateE2E();
//    return benchmarkE2E();
//    return benchmarkDrugbank();
    return loopBenchMarkWrapper();
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
//    return loopBenchMarkWrapper();
    Application app(argc, argv);
    MainTabWidget w;
    w.show();
    return app.exec();
}
