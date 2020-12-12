#include "opencv_yolo.hpp"
#include "qt_util.hpp"
#include "box2graph.hpp"
#include "mol_hw.hpp"

#include "mol3d.hpp"
#include "std_util.hpp"
#include "mol3dwindow.hpp"

#include <QFileDialog>
#include <QApplication>
#include <QDebug>

#include <iostream>
#include <exception>

using namespace std;

int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
//    qApp->setAttribute(Qt::AA_DisableHighDpiScaling);

    QApplication app(argc, argv);
    auto rootEntity = new Qt3DCore::QEntity();
    auto sceneBuilder = new Mol3D(rootEntity);
    auto view = new Mol3DWindow(rootEntity);
    auto container = QWidget::createWindowContainer(view);

//    container->showMaximized();
//    container->setFocus();

    try {
        OpenCVYolo yolo;
#ifdef Q_OS_LINUX
        yolo.init("/home/xgd/source/COCR/weights/yolov4-tiny-3l.cfg",
                  "/home/xgd/source/COCR/weights/yolov4-tiny-3l_92000.weights");
#else
        std::string path = "C:/Users/xgd/source/COCR/weights/";
        yolo.init((path + "yolov4-tiny-3l.cfg").c_str(),
                  (path + "yolov4-tiny-3l_92000.weights").c_str());
#endif
        auto fileContentReady = [&](const QString &fileName, const QByteArray &fileContent) {
            if (fileName.isEmpty()) {
                qDebug() << "Fail to find " << fileName;
                return;
            }
            // Use fileName and fileContent
            QImage image = QImage::fromData(fileContent).convertToFormat(
                    QImage::Format_RGB888);
            if (image.isNull()) {
                qDebug() << "QFileDialog::getOpenFileContent: Fail to load " << fileName;
                return;
            }
            yolo.forward(convertQImageToMat(image), true);
        };
//        QFileDialog::getOpenFileContent(QObject::tr(
//                "Image Files(*.jpg *.png *.bmp *.pgm *.pbm);;All(*.*)"), fileContentReady
//        );
        std::vector<QString> imgNameList;
        size_t index = 0;
        QObject::connect(view, &Mol3DWindow::forwardOrBackwardPressed, [&](bool isForward) {
            if (isForward) {
                index = (index + 1) % imgNameList.size();
            } else {
                index = index != 0 ? index - 1 : imgNameList.size() - 1;
            }
            QImage rawImg(imgNameList[index]);
            QImage inputImg = rawImg.convertToFormat(QImage::Format_RGB888).scaled(
                    rawImg.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            auto[gtBox, img] = yolo.forward(convertQImageToMat(inputImg), false);
            BoxGraphConverter converter(true);
            auto mols = converter.accept(gtBox, img);
            std::vector<std::string> data({"CCC", "CC", "CCCC", "C"});
            auto mol = std::make_shared<JMol>();
            mol->set(randSelect(data));
            sceneBuilder->resetMol(mol);
        });
        QImage rawImg("C:/Users/xgd/Pictures/cocr_test.png");
        QImage inputImg = rawImg.convertToFormat(QImage::Format_RGB888).scaled(
                rawImg.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        auto[gtBox, img] = yolo.forward(convertQImageToMat(inputImg), true);
        BoxGraphConverter converter(true);
        auto mols = converter.accept(gtBox, img);
        sceneBuilder->resetMol(mols[0]);
        container->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        container->showMaximized();
        return app.exec();

        for (auto &info:QDir("C:/Users/xgd/Documents/有机结构式自动构建系统/结题材料/soso17_v0/JPEGImages").entryInfoList()) {
            if ("jpg" != info.suffix()) continue;
            imgNameList.push_back(info.absoluteFilePath());
            QImage rawImg(info.absoluteFilePath());
            QImage inputImg = rawImg.convertToFormat(QImage::Format_RGB888).scaled(
                    rawImg.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            auto[gtBox, img] = yolo.forward(convertQImageToMat(inputImg), false);
            BoxGraphConverter converter(true);
            auto mols = converter.accept(gtBox, img);
        }
        container->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        container->showMaximized();
        return app.exec();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}