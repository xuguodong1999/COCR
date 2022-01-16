#include "openbabel_util.hpp"
#include "qt_mainwindow.hpp"
#include "yolo_opencv.hpp"

#include <QDir>
#include <QDebug>
#include <QMessageBox>
#include <QTranslator>
#include <QApplication>
#include <QFontDatabase>

extern std::shared_ptr<MolUtil> molUtil;
extern std::shared_ptr<YoloDetector> yoloDetector;

int main(int argc, char *argv[]) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    qApp->setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents);
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(":/simfang.subset.ttf");
    qApp->setFont(QFont("simfang"));
    QTranslator translator;
    if(translator.load(QLocale(), "COCR", "_", ":/")) {
        a.installTranslator( &translator );
    }
    try {
        molUtil = std::make_shared<MolUtilOpenBabelImpl>();
        yoloDetector = std::make_shared<YoloOpenCVImpl>();
        yoloDetector->init(":/model/yolo-3l.cfg", ":/model/yolo-3l.weights");
        (new MainWindow)->showMaximized();
        return a.exec();
    } catch (std::exception &e) {
        qDebug() << e.what();
    }
    return 0;
}

