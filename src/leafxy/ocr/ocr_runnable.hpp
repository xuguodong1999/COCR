#ifndef _XGD_OCR_RUNNABLE_HPP_
#define _XGD_OCR_RUNNABLE_HPP_

#include <QThread>
#include <memory>
#include <QPointF>
#include <QImage>
#include <opencv2/core/mat.hpp>

namespace xgd {
    class OCRManager;

    class JMol;
}
class OCRRunnablePrivate;

class OCRThread : public QThread {
Q_OBJECT
#ifdef WITH_MODEL_QRC
    inline static const QString DEFAULT_MODEL_DIR="qrc:/models/";
#else
    inline static const QString DEFAULT_MODEL_DIR = "";
#endif
public:
    explicit OCRThread(QObject *_parent = nullptr, const QString &_dir = DEFAULT_MODEL_DIR);

    ~OCRThread();

    void bindData(const QList<QList<QPointF>> &_script);

    void bindData(const QImage &_image);

    void bindData(const QPixmap &_pixmap);

    void bindData(cv::Mat &_cvMat);

    std::shared_ptr<xgd::JMol> getMol();

protected:
    void run() override;

private:
    std::shared_ptr<OCRRunnablePrivate> _p;
    std::shared_ptr<xgd::JMol> mol;
    std::shared_ptr<xgd::OCRManager> ocrManager = nullptr;
signals:

    void sig_mol_ready();
};

#endif//_XGD_OCR_RUNNABLE_HPP_
