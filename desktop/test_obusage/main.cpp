#include "openbabel_util.hpp"
#include "std_util.hpp"

#include <iostream>
#include <exception>

extern std::shared_ptr<MolUtil> molUtil;

int _main() {
    molUtil = std::make_shared<MolUtilOpenBabelImpl>();
    for (auto &choice:molUtil->getFormatChoices()) {
        std::cout << chopByFirstSpace(choice) << "\n";
    }
    std::cout << std::endl << "******" << std::endl;
    for (auto &choice:molUtil->getForcefieldChoices()) {
        std::cout << chopByFirstSpace(choice)<< "\n";
    }
    std::cout << std::endl << "******" << std::endl;
    auto jmol = molUtil->fromFormat("[H]OC1=CC=CC([C@](C2=CC([H])=CC([H])=C2)(C2=CC(C)=C([H])N=C2)[C@@](Cl)(C2=CC([H])=C[P@@H]2)C([H])([H])O[H])=N1","smi");
    std::cout<<molUtil->getFormat(jmol,"pdb")<<std::endl;
    return 0;
}
int main() {
    putenv("BABEL_DATADIR=C:/shared/openbabel3.1.1/bin/data");
    try {
        return _main();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}