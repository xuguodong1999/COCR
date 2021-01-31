#ifndef _OCR_THREAD_HPP_
#define _OCR_THREAD_HPP_

#include "mol.hpp"
#include "hw_script.hpp"

#include <QThread>

class OCRThread : public QThread {
Q_OBJECT
    HwScript script;
    const float idealItemSize = 40;// [10,120] is ok
    const int maxSize = 1280, minSize = 96;
    std::vector<std::shared_ptr<JMol>> mols;
public:
    std::vector<std::shared_ptr<JMol>> &getMols();

    void setHwScript(HwScript _hwScript);

    OCRThread(QObject *_parent = nullptr);

protected:
    void run() override;
};

#endif//_OCR_THREAD_HPP_
