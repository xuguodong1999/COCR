#pragma once

#include "XGD/Vision/Mat.h"

#include <QThread>
#include <QPointF>
#include <QtGui/QImage>

#include <memory>

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

    bool init();

Q_SIGNALS:

    void sig_mol_ready();
};