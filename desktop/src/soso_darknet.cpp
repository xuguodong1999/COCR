#include "soso_darknet.hpp"
#include "mol_op.hpp"
#include "hw_mol.hpp"
#include "isomer.hpp"
//#include "couch_data.hpp"

#include <filesystem>
#include <iostream>

SOSODarknet::SOSODarknet() : isInited(false) {
}

bool SOSODarknet::init(const char *_topDir) {
    if (!std::filesystem::exists(_topDir)) {
        if (!std::filesystem::create_directories(_topDir)) {
            std::cerr << "fail to create dir: " << _topDir << std::endl;
            exit(-1);
            return false;
        }
    }
    imgPath = _topDir + imgDir;
    labelPath = _topDir + labelDir;
    if (std::filesystem::exists(imgPath)) {
        std::filesystem::remove_all(imgPath);
    }
    if (std::filesystem::exists(labelPath)) {
        std::filesystem::remove_all(labelPath);
    }
    std::filesystem::create_directories(imgPath);
    std::filesystem::create_directories(labelPath);
    if (!std::filesystem::is_empty(imgPath) ||
        !std::filesystem::is_empty(labelPath)) {
        std::cerr << "target dir not empty: " << imgPath << " or " << labelPath << std::endl;
        exit(-1);
        return false;
    }
    isInited = true;
    return true;
}

void SOSODarknet::dump(const size_t &_numOfSamples, const size_t &_repeatTimes) {
    if (!isInited) {
        std::cerr << "you must call SOSODarknet::init before dump data" << std::endl;
        exit(-1);
    }
    auto &isomer = IsomerCounter::GetInstance();
    auto alkanes = isomer.getIsomers({
                                             3, 4, 5, 6, 7,
                                             8, 9, 10, 11, 12, 13, 14, 15
//                                             10, 11, 12, 13, 14, 15
                                     });
    auto mol = std::make_shared<JMol>();
    int loop = _numOfSamples;
    do {
        mol->setAlkane(alkanes[loop % alkanes.size()]);
        auto molOp = std::make_shared<MolOp>(mol);
        molOp->randomize();
        HwMol molItem(molOp);
        molItem.dumpAsDarknet(imgPath + std::to_string(loop),
                              labelPath + std::to_string(loop),
                              _repeatTimes);
    } while (--loop);
}
