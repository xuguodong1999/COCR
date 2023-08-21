#include "OCRRunnable.h"
#include "XGD/COCR/TextCorrector.h"
#include "XGD/COCR/GraphComposer.h"
#include "XGD/COCR/TextRecognizer.h"
#include "XGD/COCR/ObjectDetector.h"
#include "XGD/COCR/OCRManager.h"
#include "XGD/Vision/Algorithm.h"

#include <QDebug>
#include <QDir>
#include <QtWidgets/QApplication>
#include <QtGui/QScreen>

bool OCRThread::init() {
    if (ocrManager) {
        return true;
    }
    static auto detector(ObjectDetector::MakeInstance());
    static auto recognizer(TextRecognizer::MakeInstance());
    static auto corrector(std::make_shared<TextCorrector>());
    static auto composer(std::make_shared<GraphComposer>());
    if (!(detector && recognizer)) {
        return false;
    }
    ocrManager = std::make_shared<OCRManager>(
            *detector, *recognizer, *corrector, *composer);
    return ocrManager != nullptr;
}

OCRThread::OCRThread(QObject *_parent)
        : QThread(_parent) {
    qDebug() << "OCRThread::OCRThread";
}

void OCRThread::run() {
    mol = nullptr;
    if (!init()) {
        throw std::runtime_error("init failed");
    }
    try {
        mol = ocrManager->ocr(ocrManager->getImage(), false);
    } catch (std::exception &e) {
        qDebug() << __FUNCTION__ << "catch" << e.what();
    }
    if (mol) {
        Q_EMIT sig_mol_ready();
    }
}

std::shared_ptr<GuiMol> OCRThread::getMol() {
    return mol;
}

void OCRThread::bindData(const QList<QList<QPointF>> &_script) {
    if (!init()) {
        throw std::runtime_error("init failed");
    }
    ocrManager->setImage(_script, QApplication::primaryScreen()->geometry().width());
}

void OCRThread::bindData(const QImage &_image) {
    if (!init()) {
        throw std::runtime_error("init failed");
    }
    ocrManager->setImage(_image);
}

void OCRThread::bindData(const QPixmap &_pixmap) {
    if (!init()) {
        throw std::runtime_error("init failed");
    }
    ocrManager->setImage(_pixmap);
}

void OCRThread::bindData(Mat &_cvMat) {
    if (!init()) {
        throw std::runtime_error("init failed");
    }
    ocrManager->setImage(_cvMat);
}
