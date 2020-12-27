#include "openbabel_util.hpp"
#include <iostream>
#include <exception>

extern std::shared_ptr<MolUtil> molUtil;

int _main() {
    molUtil = std::make_shared<MolUtilOpenBabelImpl>();
    for (auto &choice:molUtil->getFormatChoices()) {
        std::cout << choice << std::endl;
    }
    std::cout << "******" << std::endl;
    for (auto &choice:molUtil->getForcefieldChoices()) {
        std::cout << choice << std::endl;
    }
    std::cout << "******" << std::endl;
    return 0;
}

int main() {
    _putenv("BABEL_DATADIR=C:/shared/openbabel3.1.1/bin/data");
    try {
        return _main();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}