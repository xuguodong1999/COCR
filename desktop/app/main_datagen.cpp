#include "hw_mol.hpp"
#include "mol_op.hpp"
#include <memory>
#include <isomer.hpp>

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

int main() {
    try {
        return _main();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
