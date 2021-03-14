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
    QDir dir((ROOT_DIR + "/uspto-validation-updated/CLEF").c_str());
    auto fileList = dir.entryInfoList(QDir::Filter(QDir::Files));
    if (!_badExample.empty()) {
        _random = false;
    }
    if (_random) {
        srand(time(0));
    } else {
        srand(633);
    }
    size_t idx = (_random ? (rand() % fileList.size()) : 0);
    while (idx < fileList.size()) {
        auto &file = fileList[idx];
        size_t imgIdx = idx;
        idx = (_random ? (rand() % fileList.size()) : idx + 1);
        if (file.suffix() != "png") continue;
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

#include "ui/mol2d_widget.hpp"

void testMol2D_UI() {
    using namespace xgd;
    auto mol = std::make_shared<JMolAdapter>();
    mol->readAsSMI("N[C@@](F)(C)C(=O)O");
    auto widget = new Mol2DWidget(nullptr, mol);
    widget->resize(400, 300);
    widget->show();
    widget->syncMolToScene();
}

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);
    try {
//        testJMol();
//        loopUsptoBenchMark(false, {});
//        loopUsptoBenchMark(true, {});
//        loopUsptoBenchMark(false, {25, 34, 35, 37, 49});
//        loopHwDemo();
        testMol2D_UI();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
    return app.exec();
}
