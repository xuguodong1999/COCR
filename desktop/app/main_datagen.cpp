#include "hw_mol.hpp"
#include "mol_op.hpp"
#include "isomer.hpp"
#include "soso_qsm.hpp"
#include <memory>
#include <QThreadPool>
#include <QApplication>
#include <QWidget>
#include <QDebug>

std::shared_ptr<SOSOMemory> qsm;

class Widget : public QWidget {
//    Q_OBJECT

public:
    Widget(QWidget *_parent = nullptr) : QWidget(_parent) {
        qsm = std::make_shared<SOSOMemory>("fuck0");
        QThreadPool::globalInstance()->start(qsm.get());
        connect(this,&QWidget::close,[&](){
            qsm->setStop(true);
        });
    }

    ~Widget() {
        qDebug() << __FUNCTION__ << QThread::currentThreadId();
    }
};

int _test_qsm(int argc, char **argv) {
//    qsm = std::make_shared<SOSOMemory>("fuck");
//    QThreadPool::globalInstance()->start(qsm.get());
//    return 0;
    QApplication a(argc, argv);
    (new Widget)->show();
    return a.exec();
}

int _main() {
    auto &isomer = IsomerCounter::GetInstance();
    auto alkanes = isomer.getIsomers({
//                                             3, 4, 5, 6, 7, 8, 10, 11, 12,
                                             13, 14, 15
                                     });
    JMol mol;
    for (auto &alkane:alkanes) {
        auto mol = std::make_shared<JMol>();
        mol->setAlkane(alkane);
        std::cout << mol->atomsNum() << "" << mol->bondsNum() << std::endl;
        auto molOp = std::make_shared<MolOp>(mol);
//        molOp->randomize();
        auto hwMol = std::make_shared<HwMol>(molOp);
        hwMol->showOnScreen(1);
    }
    return 0;
}

int main(int argc, char **argv) {
    try {
        return _test_qsm(argc, argv);
//        return _main();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
