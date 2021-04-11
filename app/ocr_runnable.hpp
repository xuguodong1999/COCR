#ifndef _XGD_OCR_RUNNABLE_HPP_
#define _XGD_OCR_RUNNABLE_HPP_

#include <QRunnable>
#include <memory>
#include <QPointF>
#include <QImage>

namespace xgd {
    class OCRManager;

    class JMol;
}
class OCRRunnablePrivate;

class OCRRunnable : public QRunnable {
//    Q_OBJECT

public:
    explicit OCRRunnable(const QString &_dir);

    ~OCRRunnable();

    void bindData(const QList<QList<QPointF>> &_script);

    void bindData(const QImage &_image);

    void bindData(const QPixmap &_pixmap);

    void run() override;

    std::shared_ptr<xgd::JMol> getMol();

private:
    std::shared_ptr<OCRRunnablePrivate> _p;
    std::shared_ptr<xgd::JMol> mol;
    std::shared_ptr<xgd::OCRManager> ocrManager = nullptr;
};

#endif//_XGD_OCR_RUNNABLE_HPP_
