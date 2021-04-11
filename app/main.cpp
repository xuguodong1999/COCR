#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <QDir>
#include <QDebug>
#include <QWidget>
#include <QHBoxLayout>
#include <QMainWindow>

#include <iostream>

#include "jmol_adapter.hpp"
#include "opencv_util.hpp"
#include "ocr_manager.hpp"
#include "ui/waithint_widget.h"
#include "ui/mol2d_widget.hpp"
#include "ui/mol3d_widget.hpp"
#include "ui/main_tab_widget.h"
#include "ui/main_widget.hpp"
#include "application.hpp"
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
const char *ROOT_DIR = "/home/xgd/source/repos/leafxy/";
#elif defined(Q_OS_WIN)
const char *ROOT_DIR = "C:/Users/xgd/source/repos/leafxy/";
#endif

/**
 * TODO: 1、实现简易的残基分解用于展示，按照线性->单层嵌套->多层嵌套的顺序完善，围绕 JMolAdapter
 * TODO: 2、完善 2D UI 显示，重点处理字符串对齐的问题，需求是支持残基显示为原始字符串，不要求展开
 * TODO: 3、3D UI 对残基的处理，需求是残基显示为大球、点击大球 log 残基信息
 * TODO: 4、完善结构综合
 * FIXME: 结构综合失败的 case 总结：
 * 【必解 bug】超原子一段有键，没有生成这个键
 * 【必解 bug】无意义超原子干扰视线
 * 【必解 bug】超原子多端接入
 * 【必解 bug】芳环中的圈
 * 【必解 bug】上下邻接的单原子图元
 * 【必解 bug】两个键连原子靠得近
 * 2021-4-9【进度总结】
 * 结构综合猴版上线、超原子字符串实现、出包脚本完善
 * 【TODO】
 * 优化结构综合
 * 前端 UI 全面铺开
 * 超原子字符串的动态拆解和回收逻辑
 * 字符串图元和其他图元的冲突与合并逻辑编写
 */
[[noreturn]] void loopHwDemo() {
    auto ocrManager = makeOCRManager(ROOT_DIR + QString("resources/model").toStdString());
    std::vector<int> testcases = {1, 2, 3, 4, 5, 6, 7};
//    std::vector<int> testcases = {7};
    size_t idx = 0;
    while (true) {
        QImage image((ROOT_DIR + QString("testcase/%1.jpg").arg(testcases[idx++])));
        idx %= testcases.size();
        auto testImg = xgd::convertQImageToMat(image);
//        auto testImg = cv::imread(ROOT_DIR + "testcase/1.jpg", cv::IMREAD_GRAYSCALE);
//        cv::erode(testImg, testImg, cv::Mat());
        auto mol = ocrManager.ocr(testImg, true);
        auto widget = new Mol2DWidget(nullptr, mol);
        widget->resize(960, 640);
        widget->show();
        widget->syncMolToScene();
//        qDebug() << "*****************  result  *****************\n" << mol->writeAsSMI().c_str();
        cv::waitKey(0);
    }
}

void loopUsptoBenchMark(bool _random = false, const QSet<size_t> &_badExample = {}) {
    auto ocrManager = makeOCRManager(ROOT_DIR + QString("resources/model").toStdString());
    QDir dir(ROOT_DIR + QString("testcase/public/USPTO"));
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
        if (file.suffix() != "TIF" && file.suffix() != "png") { continue; }
        if (!_badExample.empty() && !_badExample.contains(imgIdx)) { continue; }
        qDebug() << imgIdx << ":" << file.fileName();
        cv::Mat image = cv::imread(file.absoluteFilePath().toStdString(), cv::IMREAD_GRAYSCALE);
//        cv::erode(image, image, cv::Mat());
        int angle = 2;
        if (rand() % 2) { angle = -2; }
        image = xgd::rotateCvMat(image, angle);
        auto mol = ocrManager.ocr(image, true);
        auto widget = new Mol2DWidget(nullptr, mol);
        widget->resize(960, 640);
        widget->show();
        widget->syncMolToScene();
        cv::waitKey(0);
    }
}

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


void testMol2D_UI() {
    auto mol = getTestMol();
    auto widget = new Mol2DWidget(nullptr, mol);
    widget->resize(960, 640);
    widget->show();
    widget->syncMolToScene();
}


void testMol3D_UI() {
    auto mol = getTestMol();
//    auto mol = std::make_shared<xgd::JMolAdapter>();
//    mol->readAsSMI("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC");
//    mol->readAsSMI("C([H])([H])=C([H])-C#N");
//    mol->readAsSMI("c1([H])c([H])c([H])c(O[H])c(C#N)c1([H])");
//    mol->readAsSMI("c1([H])c([H])c([H])c([H])c([H])c1([H])");

//    auto mw = new QMainWindow();
//    mw->setAttribute(Qt::WA_AcceptTouchEvents);
//    mw->grabGesture(Qt::PinchGesture);
    auto w = new Mol3DWidget(nullptr, mol);
//    mw->setCentralWidget(w);
    w->resize(1080, 720);
    w->show();
    w->syncMolToScene();
}


int main(int argc, char *argv[]) {
    // FIXME: Qt Android 的 QWidget 部分在 HighDpi 下大小混乱
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0) && not defined(Q_OS_ANDROID)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    Application app(argc, argv);
    try {
//        testJMol();
//        loopUsptoBenchMark(false, {});
//        loopUsptoBenchMark(true, {});
//        loopUsptoBenchMark(false, {25, 34, 35, 37, 49});
//        loopHwDemo();
//        testMol2D_UI();
//        testMol3D_UI();
//        (new GestureWidget)->show();
//        auto w=new QWidget();
//        w->resize(640,320);
//        auto wait=new WaitHintWidget(w);
//        w->show();
//        wait->startWaitHint();
//        (new MainWidget)->show();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    MainTabWidget w;
    w.show();
    return app.exec();
}
