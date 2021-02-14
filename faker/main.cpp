#include "hw_mol.hpp"
#include "mol_op.hpp"
#include "isomer.hpp"
#include "colors.hpp"
#include "polya.hpp"
#include "std_util.hpp"
#include "crnn_data.hpp"
#include "hw_data.hpp"
#include "soso_darknet.hpp"
#include "linetextdata.hpp"
#include <random>
#include <memory>
#include <QCoreApplication>
#include <opencv2/highgui.hpp>

int _main();

int dump();

int gen_str_text() {
    LineTextDataCreator lc;
//    lc.loadFromSuperAtom();
//    lc.loadFromWordDict();
//    lc.displaySet();
    lc.loadFromPattern();
    return 0;
}

#include <opencv2/imgproc.hpp>

int gen_str() {
    srand(22);
    HwDataLoader::getInstance();
    int w = 640;
    int h = 32;
    cv::Point2f center(w / 2, h / 2);
    cv::Mat mat(h, w, CV_8UC3, getScalar(ColorName::rgbWhite));
    HwStr hwStr;
//    hwStr.loadRichACSII("C12H25COO-CH2CH2-C#C-COOEt");
//    hwStr.loadRichText({"C", "H", "3", "C", "O", "O", NEG_CHARGE_TEXT});
    hwStr.loadRichText({POS_CHARGE_TEXT, "N", "#", "N", "-", "N", "O", "3", NEG_CHARGE_TEXT});
//    HwStr hwStr("1111111111");
    hwStr.moveCenterTo(center);
    hwStr.resizeTo(w - 4, h - 4);
    HwController hwController(1);
    hwStr.setHwController(hwController);
    hwStr.paintTo(mat);

    cv::imshow("wtf", mat);
    cv::waitKey(0);
    return 0;
}

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    try {
//        return dump();
//        return _main();
//        return gen_str();
//        return gen_str_text();
        CRNNDataGenerator crnnDataGenerator;
//        crnnDataGenerator.display();
//        crnnDataGenerator.getDictTexts();
        crnnDataGenerator.init("/home/xgd/source/repos/jokejoker/data/soso-full");
//        crnnDataGenerator.display();
        crnnDataGenerator.dump();
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}

int dump() {
    HwDataLoader::getInstance();
    SOSODarknet sd;
    sd.init("/media/xgd/hjyy-ext4/soso17_small");
    sd.dump(1000, 4);
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
    for (size_t i = 0; i < full_size; i++) {
        auto mol = std::make_shared<JMol>();
        mol->setAlkane(alkanes[i % alkanes.size()]);
        auto molOp = std::make_shared<MolOp>(mol);
        //const float &_addHydrogenProb, bool _replaceBond,
        //        bool _replaceAtom, bool _addAromaticR+ing, bool _addCommonRing
//        molOp->randomize(0.1,byProb(0.95),byProb(0.95),byProb(0.5),byProb(0.5));
        auto hwMol = std::make_shared<HwMol>(molOp);
        hwMol->showOnScreen(1);
    }
    return 0;
}