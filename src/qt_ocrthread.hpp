#ifndef _OCR_THREAD_HPP_
#define _OCR_THREAD_HPP_

#include "mol.hpp"
#include "hw_script.hpp"
#include <QThread>
#include <QImage>

class OCRThread : public QThread {
Q_OBJECT
    HwScript script;
    QImage qImage;
    const float idealItemSize = 80;
    const int maxSize = 640, minSize = 64;
    std::vector<std::shared_ptr<JMol>> mols;
public:
    std::vector<std::shared_ptr<JMol>> &getMols();

    void setHwScript(HwScript _hwScript);

    void setQImage(QImage&&_qImage);

    OCRThread(QObject *_parent = nullptr);

protected:
    void run() override;
};

#endif
