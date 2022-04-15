#include "ocr_runnable.h"
#include "cocr/text_corrector.h"
#include "cocr/graph_composer.h"
#include "cocr/text_recognizer.h"
#include "cocr/object_detector.h"
#include "ocv/algorithm.h"
#include <QDebug>
#include <QDir>
#include <QApplication>
#include <QDesktopWidget>
#include <QScreen>
#include "cocr/ocr_manager.h"


OCRThread::OCRThread(QObject *_parent)
        : QThread(_parent) {
    qDebug() << "OCRThread::OCRThread";
    static auto detector = ObjectDetector::MakeInstance();
    static auto recognizer = TextRecognizer::MakeInstance();
    static auto corrector = std::make_shared<TextCorrector>();
    static auto composer = std::make_shared<GraphComposer>();
    if (detector && recognizer) {
        ocrManager = std::make_shared<OCRManager>(
                *detector, *recognizer, *corrector, *composer);
    } else {
        exit(EXIT_FAILURE);
    }
}

void OCRThread::run() {
    try {
        mol = ocrManager->ocr(ocrManager->getImage(), false);
    } catch (std::exception &e) {
        qDebug() << __FUNCTION__ << "catch" << e.what();
        mol = nullptr;
    }
    if (mol) {
        emit sig_mol_ready();
    }
}

std::shared_ptr<GuiMol> OCRThread::getMol() {
    return mol;
}

void OCRThread::bindData(const QList<QList<QPointF>> &_script) {
    ocrManager->setImage(_script, QApplication::desktop()->width());
}

void OCRThread::bindData(const QImage &_image) {
    ocrManager->setImage(_image);
}

void OCRThread::bindData(const QPixmap &_pixmap) {
    ocrManager->setImage(_pixmap);
}

void OCRThread::bindData(Mat &_cvMat) {
    ocrManager->setImage(_cvMat);
}
