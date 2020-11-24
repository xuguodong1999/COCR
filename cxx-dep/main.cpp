#include <GraphMol/SmilesParse/SmilesParse.h>
#include <GraphMol/SmilesParse/SmilesWrite.h>
#include <GraphMol/FileParsers/FileParsers.h>
#include <GraphMol/GraphMol.h>
#include <GraphMol/MolOps.h>
#include <GraphMol/Depictor/RDDepictor.h>
#include <GraphMol/DistGeomHelpers/Embedder.h>
#include <GraphMol/ForceFieldHelpers/MMFF/MMFF.h>

#include <opencv2/opencv.hpp>

#include <torch/torch.h>

#include <QFontDatabase>
#include <QTranslator>
#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <iostream>
#include <string>

// void test2();
void test1()
{
    cv::Mat mat;
    std::cout << mat.rows << "x" << mat.cols << std::endl;
}
void test0()
{
    std::shared_ptr<RDKit::ROMol> mol1(RDKit::SmilesToMol("C1CCC1"));
    std::cout << RDKit::MolToMolBlock(*mol1) << std::endl;

    mol1->setProp("_Name", "cyclobutane");
    std::cout << RDKit::MolToMolBlock(*mol1) << std::endl;

    RDDepict::compute2DCoords(*mol1);
    std::cout << RDKit::MolToMolBlock(*mol1) << std::endl;

    std::shared_ptr<RDKit::ROMol> mol2(RDKit::SmilesToMol("C1CCC1"));
    mol2->setProp("_Name", "cyclobutane3D");
    RDKit::DGeomHelpers::EmbedMolecule(*mol2);
    RDKit::MMFF::MMFFOptimizeMolecule(*mol2, 1000, "MMFF94s");
    std::cout << RDKit::MolToMolBlock(*mol2) << std::endl;

    std::shared_ptr<RDKit::ROMol> mol3(RDKit::MolOps::addHs(*mol2));
    RDKit::MMFF::MMFFOptimizeMolecule(*mol3, 1000, "MMFF94s");
    std::cout << RDKit::MolToMolBlock(*mol3) << std::endl;

    std::shared_ptr<RDKit::RWMol> mol4(new RDKit::RWMol(*mol3));
    RDKit::MolOps::addHs(*mol4);

    std::shared_ptr<RDKit::ROMol> mol3sp(RDKit::MolOps::addHs(*mol2));
    mol3sp->setProp("_Name", "cyclobutaneSP");
    RDKit::MMFF::MMFFOptimizeMolecule(*mol3sp, 1000, "MMFF94s");
    std::cout << RDKit::MolToMolBlock(*mol3sp) << std::endl;

    std::shared_ptr<RDKit::ROMol> mol5(RDKit::MolOps::removeHs(*mol3));
    RDKit::MolOps::removeHs(*mol4);
}

inline void addFontData()
{
    QFontDatabase database;
    int id = QFontDatabase::addApplicationFont(":/simfang.subset.ttf");
    try
    {
        qApp->setFont(QFont(database.applicationFontFamilies(id).at(0)));
    }
    catch (std::exception e)
    {
        qWarning() << e.what();
    }
}

inline void addTranslator()
{
    static QTranslator translator;
    if (translator.load(":/demo_zh_CN.qm"))
    {
        if (!qApp->installTranslator(&translator))
            qWarning() << "fail to load " << translator.filePath();
    }
}
#include <QApplication>
#include <QWidget>
int main(int argc, char **argv)
{
    test0();
    test1();
    // test2();
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    addFontData();
    addTranslator();
    // QQmlApplicationEngine engine;
    // const QUrl url(QStringLiteral("qrc:/main.qml"));
    // QObject::connect(&engine, &QQmlApplicationEngine::objectCreated, &app,
    //                  [url](QObject *obj, const QUrl &objUrl) {
    //                      if (!obj && url == objUrl)
    //                          QCoreApplication::exit(-1);
    //                  }, Qt::QueuedConnection);
    // engine.load(url);
    (new QWidget)->show();
    try
    {
        auto tensor = torch::tensor({1, 2, 3});
        std::cout << tensor << std::endl;
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    std::cout << "leave torch test" << std::endl;
    return app.exec();
}
