#include "hw_mol.hpp"
#include "mol_op.hpp"
#include "isomer.hpp"
#include "soso_mem.hpp"
#include <memory>
#include <QThreadPool>
#include <QApplication>
#include <QWidget>
#include <QDebug>


int _test_qsm(int argc, char **argv) {
//    QSharedMemory qsm("fuck");
//    qsm.attach();
//    return 0;
    QApplication a(argc, argv);
    auto mcWidget=new MCWidget();
    mcWidget->show();
    return a.exec();
}

int _main();

int main(int argc, char **argv) {
    try {
        return _test_qsm(argc, argv);
//        return _main();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
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