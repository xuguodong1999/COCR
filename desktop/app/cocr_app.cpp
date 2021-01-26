#include "mol3d.hpp"
#include "mol3d_builder.hpp"
#include "mol3dwindow.hpp"
#include "openbabel_util.hpp"
#include <QApplication>
#include <QWidget>
#include <Qt3DCore/QEntity>
#include <QDebug>
#include <iostream>
#include <opencv2/imgcodecs.hpp>

#include "yolo_ncnn.hpp"
#include "yolo_opencv.hpp"
extern std::string WORKSPACE;
extern std::vector<std::string> CLASSES;

int main(int argc, char *argv[]) {
    try {
        std::shared_ptr<YoloDetector> detector = std::make_shared<YoloOpenCVImpl>();
        detector->init((WORKSPACE + "/yolov4-smallest-3l.cfg").c_str(),
                       (WORKSPACE + "/yolov4-smallest-3l_400000.weights").c_str());
//        detector = std::make_shared<YoloNCNNImpl>();
//        detector->init((WORKSPACE + "/cocr17.int8.param").c_str(),
//                       (WORKSPACE + "/cocr17_400000.int8.bin").c_str());
        cv::Mat img = cv::imread("C:/Users/xgd/Pictures/alkane.png");
        detector->detectAndDisplay(img, CLASSES);
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    return 0;
    std::srand(0);
#ifdef Q_OS_WIN64
    _putenv("BABEL_DATADIR=C:/static/openbabel-3.1.1/data");
#endif
//    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    qApp->setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents);
    QApplication a(argc, argv);
    auto molUtil = std::make_shared<MolUtilOpenBabelImpl>();
    //C1C(C=C)CC(C#C)C(C#C)C1
    //[H]OC1=CC=CC([C@](C2=CC([H])=CC([H])=C2)(C2=CC(C)=C([H])N=C2)[C@@](Cl)(C2=CC([H])=C[P@@H]2)C([H])([H])O[H])=N1
    //C1C2CN3CC1CC(C2)C3
    auto mol = molUtil->fromFormat("C=CC=CC=CC=CC#C", "smi");
    auto rootEntity = new Qt3DCore::QEntity();
    auto molBuilder = new Mol3DBuilder(rootEntity, mol);
    if (!molBuilder->build())exit(-1);
    auto view = new Mol3DWindow(rootEntity);
    auto container = QWidget::createWindowContainer(view);
    container->setAttribute(Qt::WidgetAttribute::WA_AcceptTouchEvents);
    container->showMaximized();
//    qDebug() << mol->atomsNum() << "," << mol->bondsNum();
//    std::cout << molUtil->getFormat(*mol, "pdb") << std::endl;
    return a.exec();
}
