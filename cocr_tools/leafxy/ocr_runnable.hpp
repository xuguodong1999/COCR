#pragma once

#include <QThread>
#include <memory>
#include <QPointF>
#include <QImage>
#include <opencv2/core/mat.hpp>

namespace cocr {
    class OCRManager;

    class JMol;
}

class OCRThread : public QThread {
Q_OBJECT
public:
    explicit OCRThread(QObject *_parent = nullptr);

    void bindData(const QList<QList<QPointF>> &_script);

    void bindData(const QImage &_image);

    void bindData(const QPixmap &_pixmap);

    void bindData(cv::Mat &_cvMat);

    std::shared_ptr<cocr::JMol> getMol();

protected:
    void run() override;

private:
    std::shared_ptr<cocr::JMol> mol;
    std::shared_ptr<cocr::OCRManager> ocrManager = nullptr;
signals:

    void sig_mol_ready();
};