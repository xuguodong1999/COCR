#include "Impl/HandwritingMol.h"
#include "Impl/MolOp.h"

#include "XGD/Data/ChemStructImage.h"
#include "XGD/Math/GreedyIsomerSolver.h"
#include "XGD/Base/Util.h"

#include <filesystem>
#include <iostream>
#include <random>

using namespace data_deprecated;

SOSODarknet::SOSODarknet() : isInited(false) {
}

bool SOSODarknet::init(const std::string &_topDir) {
    if (!std::filesystem::exists(_topDir)) {
        if (!std::filesystem::create_directories(_topDir)) {
            xgd::serr << "fail to create dir: " << _topDir << std::endl;
            exit(-1);
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
        xgd::serr << "target dir not empty: " << imgPath << " or " << labelPath << std::endl;
        exit(-1);
    }
    isInited = true;
    return true;
}

void SOSODarknet::dump(const size_t &_numOfSamples, const size_t &_repeatTimes) {
    if (!isInited) {
        xgd::serr << "you must call SOSODarknet::init before dump data" << std::endl;
        exit(-1);
    }
    auto &isomer = IsomerCounter::GetInstance();
    auto alkanes = isomer.getIsomers({
                                             2, 3, 4, 5,
                                             6, 7,
                                             8, 9, 10, 11, 12, 13,
                                             14, 15, 16
                                     });
    const int loopTime = _numOfSamples / _repeatTimes;
    std::shuffle(alkanes.begin(), alkanes.end(), std::default_random_engine());
//#pragma omp parallel for num_threads(8)
    for (int i = 0; i < loopTime; i++) {
        std::shared_ptr<HwMol> hwMol(nullptr);
        if (StdUtil::byProb(0.1)) {
            hwMol = HwMol::GetSpecialExample(0.1);
        } else {
            auto mol = std::make_shared<JMol>();
            mol->setAlkane(alkanes[i % alkanes.size()]);
            auto molOp = std::make_shared<MolOp>(mol);
            bool add_aro = StdUtil::byProb(0.5), add_com = StdUtil::byProb(0.5);
            if (alkanes[i % alkanes.size()].length() > 40) {
                add_com = false;
            }
            if (alkanes[i % alkanes.size()].length() > 30) {
                add_aro = false;
            }
            molOp->randomize(0.1, StdUtil::byProb(0.95), StdUtil::byProb(0.95),
                             add_aro, add_com);
            hwMol = std::make_shared<HwMol>(molOp);
        }
        if (hwMol) {
            auto idxStr = std::to_string(i);
            auto subDir = "/" + std::to_string(i % 100) + "/";
            if (!std::filesystem::exists(imgPath + subDir)) {
                std::filesystem::create_directory(imgPath + subDir);
            }
            if (!std::filesystem::exists(labelPath + subDir)) {
                std::filesystem::create_directory(labelPath + subDir);
            }
            hwMol->dumpAsDarknet(imgPath + subDir + idxStr,
                                 labelPath + subDir + idxStr, _repeatTimes);
        }
        if (i % 5000 == 0) {
            xgd::sout << i << std::endl;
        }
    }
}

void SOSODarknet::display() {
    auto &isomer = IsomerCounter::GetInstance();
    auto alkanes = isomer.getIsomers({
                                             2, 3, 4, 5, 6, 7,
                                             8, 9, 10, 11, 12, 13, 14, 15,
//                                             16
                                     });
    const int loopTime = 10000;
    std::shuffle(alkanes.begin(), alkanes.end(), std::default_random_engine());
    for (int i = 0; i < loopTime; i++) {
        std::shared_ptr<HwMol> hwMol(nullptr);
        if (StdUtil::byProb(0.1)) {
            hwMol = HwMol::GetSpecialExample(0.1);
        } else {
            auto mol = std::make_shared<JMol>();
            mol->setAlkane(alkanes[i % alkanes.size()]);
            auto molOp = std::make_shared<MolOp>(mol);
            bool add_aro = StdUtil::byProb(0.5), add_com = StdUtil::byProb(0.5);
            if (alkanes[i % alkanes.size()].length() > 40) {
                add_com = false;
            }
            if (alkanes[i % alkanes.size()].length() > 30) {
                add_aro = false;
            }
            molOp->randomize(0.1, StdUtil::byProb(0.95), StdUtil::byProb(0.95),
                             add_aro, add_com);
            hwMol = std::make_shared<HwMol>(molOp);
        }
        if (hwMol) {
            hwMol->showOnScreen(1, true);
        }
    }
}