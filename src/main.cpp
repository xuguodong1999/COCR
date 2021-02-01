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

bool release(const QString &_qrcTopDir, const QString &_targetDir);

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
    QString obdataDir = qApp->applicationDirPath() + "/obdata";
    if (!release(":/obdata", obdataDir)) {
        QMessageBox::information(
                nullptr, "Warning", "Fail to release openbabel data", QMessageBox::Yes);
    }
#ifdef Q_OS_WIN64
    _putenv(("BABEL_DATADIR=" + obdataDir).toLocal8Bit().data());
#elif defined(Q_OS_LINUX)
    putenv(("BABEL_DATADIR=" + obdataDir).toLocal8Bit().data());
#endif
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

bool release(const QString &_qrcTopDir, const QString &_targetDir) {
    QDir qrcTopDir(_qrcTopDir);
    if (!qrcTopDir.exists()) {
        return false;
    }
    QDir targetDir(_targetDir);
    if (!targetDir.exists()) {
        if (!targetDir.mkpath(_targetDir)) {
            return false;
        }
    }
    QFileInfoList fileInfoList = qrcTopDir.entryInfoList();
    while (!fileInfoList.isEmpty()) {
        QFileInfo tempFileInfo = fileInfoList.last();
        if (tempFileInfo.isFile()) {
            const QString newFilePath = _targetDir + "/" + tempFileInfo.fileName();
            if (!QFile::exists(newFilePath)) {
                QFile::copy(tempFileInfo.absoluteFilePath(), newFilePath);
            }
            if (!QFile::exists(newFilePath)) {
                return false;
            }
            fileInfoList.removeLast();
        } else if (tempFileInfo.isDir()) {
            if (tempFileInfo.fileName() != "." && tempFileInfo.fileName() != "..") {
                QDir subDir(tempFileInfo.filePath());
                fileInfoList.removeLast();
                fileInfoList.append(subDir.entryInfoList());
            }
        } else {
            fileInfoList.removeLast();
        }
    }
    return true;
}
