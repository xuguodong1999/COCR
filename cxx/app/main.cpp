#include "opencv_yolo.hpp"
#include "qt_util.hpp"
#include "box2graph.hpp"
#include "mol_hw.hpp"

#include <QFileDialog>
#include <QApplication>
#include <QDebug>
#include <QToolButton>

#include <iostream>
#include <exception>

using namespace std;

int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);

    QApplication app(argc, argv);
    try {
        OpenCVYolo yolo;
        std::string path = "C:/Users/xgd/Downloads/soso17-2020-12-01/";
        yolo.init((path + "yolov4-tiny-3l.cfg").c_str(),
                  (path + "yolov4-tiny-3l_92000.weights").c_str());
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
        QDir dir("D:/soso17/JPEGImages");
        for (auto &info:dir.entryInfoList()) {
            if ("jpg" != info.suffix()) continue;
            qDebug() << info.filePath();
            QImage image = QImage(info.absoluteFilePath()).convertToFormat(
                    QImage::Format_RGB888);
            auto[gtBox, img]=yolo.forward(convertQImageToMat(image.scaled(
                    image.size(), Qt::IgnoreAspectRatio,
                    Qt::SmoothTransformation)), true);
            JMol mol;
            BoxGraphConverter converter(mol);
            converter.accept(gtBox, img);
        }
        return app.exec();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}