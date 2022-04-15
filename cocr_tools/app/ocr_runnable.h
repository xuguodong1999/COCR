#pragma once

#include <QThread>
#include <memory>
#include <QPointF>
#include <QImage>
#include "ocv/mat.h"

class OCRManager;

class GuiMol;


class OCRThread : public QThread {
Q_OBJECT
public:
    explicit OCRThread(QObject *_parent = nullptr);

    void bindData(const QList<QList<QPointF>> &_script);

    void bindData(const QImage &_image);

    void bindData(const QPixmap &_pixmap);

    void bindData(Mat &_cvMat);

    std::shared_ptr<GuiMol> getMol();

protected:
    void run() override;

private:
    std::shared_ptr<GuiMol> mol;
    std::shared_ptr<OCRManager> ocrManager = nullptr;
signals:

    void sig_mol_ready();
};