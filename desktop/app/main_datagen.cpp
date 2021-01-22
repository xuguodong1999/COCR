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
    auto mcWidget = new MCWidget();
    mcWidget->show();
    return a.exec();
}

int _main();
int dump();
int main(int argc, char **argv) {
    try {
        return dump();
//        return _test_qsm(argc, argv);
//        return _main();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}

#include "polya.hpp"
#include "std_util.hpp"
#include "hw_data.hpp"
#include <random>
#include "soso_darknet.hpp"
int dump(){
    HwDataLoader::getInstance();
    SOSODarknet sd;
    sd.init("/media/xgd/hjyy-ext4/soso17_small");
    sd.dump(1000,4);
    return 0;
}
int _main() {
//    auto& ic=PolyaIsomerCounter::GetInstance();
//    ic.count(18);
    auto &isomer = IsomerCounter::GetInstance();
    auto alkanes = isomer.getIsomers({
                                             2, 3, 4, 5, 6, 7, 8, 10,
//                                             11, 12, 13
                                     });
    std::cout << alkanes.size() << std::endl;
//    exit(-1);
    size_t full_size = 400000;
    for(size_t i=0;i<full_size;i++){
        auto mol = std::make_shared<JMol>();
        mol->setAlkane(alkanes[i%alkanes.size()]);
        auto molOp = std::make_shared<MolOp>(mol);
        //const float &_addHydrogenProb, bool _replaceBond,
        //        bool _replaceAtom, bool _addAromaticRing, bool _addCommonRing
//        molOp->randomize(0.1,byProb(0.95),byProb(0.95),byProb(0.5),byProb(0.5));
        auto hwMol = std::make_shared<HwMol>(molOp);
        hwMol->showOnScreen(1);
    }
    return 0;
}