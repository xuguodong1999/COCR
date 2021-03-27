#include <QDir>
#include <QDebug>
#include <QWidget>
#include <QApplication>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include "opencv_util.hpp"
#include "ocr_manager.hpp"
#include "ncnn_impl/text_recognizer_ncnn_impl.hpp"

//#undef HAVE_OPENCV_DNN
#if defined(HAVE_OPENCV_DNN) && !defined(Q_OS_ANDROID)

#include "opencv_dnn_impl/object_detector_opencv_impl.hpp"
#include "opencv_dnn_impl/text_recognizer_opencv_impl.hpp"// unused, only for performance test

#else

#include "ncnn_impl/object_detector_ncnn_impl.hpp"

#endif

xgd::OCRManager makeOCRManager(const std::string &_modelDir) {
    /// detector
#if defined(HAVE_OPENCV_DNN) && !defined(Q_OS_ANDROID) && !defined(Q_OS_IOS)
    static xgd::ObjectDetectorOpenCVImpl detector;
    if (!detector.initModel(
            _modelDir + "/yolo-3l-c8.cfg",
            _modelDir + "/yolo-3l-c8.weights")) {
        std::cerr << "fail to load yolo from opencv" << std::endl;
    }
    detector.setConfThresh(0.15);
    detector.setIouThresh(0.45);
#else
    static xgd::ObjectDetectorNcnnImpl detector;
    detector.setNumThread(4);
    if (!detector.initModel(_modelDir + "/yolo_3l_c8.bin",
                            _modelDir + "/yolo_3l_c8.param",
                            1280)) {
        std::cerr << "fail to load yolo from ncnn" << std::endl;
    }
#endif
    /// recognizer
    static xgd::TextRecognizerNcnnImpl recognizer;
    if (!recognizer.initModel(_modelDir + "/vgg_lstm_57_fp16_mixFont.bin",
                              _modelDir + "/vgg_lstm_57_fp16.param",
                              xgd::TextCorrector::GetAlphabet(), 3200)) {
        std::cerr << "fail to load crnn from ncnn" << std::endl;
    }
//    /// recognizer by opencv_dnn
//    static xgd::TextRecognizerOpenCVImpl recognizer;
//    if (!recognizer.initModel(_modelDir + "/crnn_192_mix_sim.onnx",
//                              xgd::TextCorrector::GetAlphabet(), 192)) {
//        std::cerr << "fail to load crnn from ncnn" << std::endl;
//    }
    /// corrector
    static xgd::TextCorrector corrector;
    corrector.InitData();
    /// composer
    static xgd::GraphComposer composer;
    /// manager
    xgd::OCRManager ocrManager(detector, recognizer, corrector, composer);
    return ocrManager;
}

#ifdef Q_OS_LINUX
std::string ROOT_DIR = "/home/xgd/source/repos/jokejoker/workspace/";
#elif defined(Q_OS_WIN)
std::string ROOT_DIR = "C:/Users/xgd/source/repos/jokejoker/workspace/";
#endif

void loopHwDemo() {
    auto ocrManager = makeOCRManager(ROOT_DIR + "../resources/model");
    while (true) {
        auto testImg = cv::imread(ROOT_DIR + "/demo-o1.jpg", cv::IMREAD_GRAYSCALE);
//        cv::erode(testImg, testImg, cv::Mat());
        ocrManager.ocr(testImg, true);
    }
}

void loopUsptoBenchMark(bool _random = false, const QSet<size_t> &_badExample = {}) {
    auto ocrManager = makeOCRManager(ROOT_DIR + "../resources/model");
    QDir dir((ROOT_DIR + "/uspto-validation-updated/images").c_str());
    auto fileList = dir.entryInfoList(QDir::Filter(QDir::Files));
    if (!_badExample.empty()) {
        _random = false;
    }
    if (_random) {
        srand(time(0));
    } else {
        srand(0);
    }
    size_t idx = (_random ? (rand() % fileList.size()) : 0);
    while (idx < fileList.size()) {
        auto &file = fileList[idx];
        size_t imgIdx = idx;
        idx = (_random ? (rand() % fileList.size()) : idx + 1);
        if (file.suffix() != "TIF") continue;
        if (!_badExample.empty() && !_badExample.contains(imgIdx)) continue;
        qDebug() << imgIdx << ":" << file.fileName();
        cv::Mat image = cv::imread(file.absoluteFilePath().toStdString(), cv::IMREAD_GRAYSCALE);
//        cv::erode(image, image, cv::Mat());
        int angle = 2;
        if (rand() % 2)angle = -2;
        image = xgd::rotateCvMat(image, angle);
        ocrManager.ocr(image, true);
    }
}

#include "jmol_adapter.hpp"

void testJMol() {
    using namespace xgd;
    JMolAdapter mol;
    mol.readAsSMI("c1ccccc1");
    std::cout << mol.writeAsPDB() << std::endl;
    mol.display();
    std::cout << "********************\n";
    mol.removeAtom(0);
    auto b01 = mol.removeBond(0);
    auto b02 = mol.removeBond(5);
    auto newAtom = mol.addAtom(ElementType::O);
    auto b1 = mol.addBond(newAtom, mol.getAtom(1), b01->getType());
    auto b2 = mol.addBond(newAtom, mol.getAtom(5), b01->getType());
    mol.display();
}

std::shared_ptr<xgd::JMol> getTestMol() {
    using namespace xgd;
    auto mol = std::make_shared<JMolAdapter>();
//    mol->readAsSMI("N[C@@](F)(C)C(=O)O");
    auto c0 = mol->addAtom(ElementType::C);
    auto c01 = mol->addAtom(ElementType::C);
    auto c1 = mol->addAtom(ElementType::C);
    auto c2 = mol->addAtom(ElementType::C);
    auto c3 = mol->addAtom(ElementType::C);
    auto c4 = mol->addAtom(ElementType::C);
    auto c5 = mol->addAtom(ElementType::C);
    auto o2 = mol->addAtom(ElementType::O);
    auto n1 = mol->addAtom(ElementType::N);
    auto c6 = mol->addAtom(ElementType::C);
    auto f1 = mol->addAtom(ElementType::F);
    auto cl1 = mol->addAtom(ElementType::Cl);
    auto br1 = mol->addAtom(ElementType::Br);
    auto i1 = mol->addAtom(ElementType::I);
    auto nc = mol->addBond(c2, n1, BondType::UpBond);
    auto nc6 = mol->addBond(c6, n1, BondType::DoubleBond);
    auto cc1 = mol->addBond(c2, c1);
    auto cf = mol->addBond(c2, f1, BondType::DownBond);
    auto cc2 = mol->addBond(c3, c2);
    auto cc3 = mol->addBond(c0, c1, BondType::TripleBond);
    auto cc4 = mol->addBond(c0, c01);
    auto ccl = mol->addBond(c01, cl1, BondType::ImplicitBond);
    auto cbr = mol->addBond(c01, br1, BondType::ImplicitBond);
    auto ci = mol->addBond(c01, i1);
    auto c3c4 = mol->addBond(c3, c4, BondType::DelocalizedBond);
    auto c4c5 = mol->addBond(c4, c5, BondType::DelocalizedBond);
    auto co2 = mol->addBond(c3, o2, BondType::DelocalizedBond);
    return mol;
}

#include "ui/mol2d_widget.hpp"

void testMol2D_UI() {
    auto mol = getTestMol();
    auto widget = new Mol2DWidget(nullptr, mol);
    widget->resize(960, 640);
    widget->show();
    widget->syncMolToScene();
}

#include "ui/mol3d_widget.hpp"
#include <QMainWindow>

void testMol3D_UI() {
    auto mol = std::make_shared<xgd::JMolAdapter>();
//    mol->readAsSMI("c1([H])c([H])c([H])c(O[H])c(C#N)c1([H])");
//    mol->readAsSMI("c1([H])c([H])c([H])c([H])c([H])c1([H])");
    mol->readAsSMI("C([H])([H])=C([H])-C#N");
//    auto mol = getTestMol();
//    auto mw = new QMainWindow();
//    mw->setAttribute(Qt::WA_AcceptTouchEvents);
//    mw->grabGesture(Qt::PinchGesture);
    auto w = new Mol3DWidget(nullptr, mol);
//    mw->setCentralWidget(w);
    w->resize(1080, 720);
    w->show();
    w->syncMolToScene();
}

#include "ui/waithint_widget.h"
#include <QHBoxLayout>


int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
//    qApp->setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents);
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_NativeWindows);
    try {
//        testJMol();
//        loopUsptoBenchMark(false, {});
//        loopUsptoBenchMark(true, {});
//        loopUsptoBenchMark(false, {25, 34, 35, 37, 49});
//        loopHwDemo();
//        testMol2D_UI();
        testMol3D_UI();
//        (new GestureWidget)->show();
//        auto w=new QWidget();
//        w->resize(640,320);
//        auto wait=new WaitHintWidget(w);
//        w->show();
//        wait->startWaitHint();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return app.exec();
}
