#include <iostream>

using namespace std;
//#include "torch_util.hpp"
//#include "torch_crnn.hpp"
//void testCRNN() {
//    auto imgTensor = loadImgTensor("C:/Users/xgd/Pictures/crnn_test.png");
//    std::cout << "imgTensor.size=" << imgTensor.sizes() << std::endl;
//    auto crnn = std::make_shared<CRNN>(6947);
//    auto output = crnn->forward(imgTensor);
//    std::cout << "outSize=" << output.sizes() << std::endl;
//}

#include "hw_mol.hpp"
#include "isomer.hpp"
#include "couch_data.hpp"
int testHWDraw() {
    CouchLoader::LoadCouchDataSet(false);
    auto &isomer = IsomerCounter::GetInstance();
    auto alkanes = isomer.getIsomers({
                                             3, 4, 5, 6, 7, 8,
                                             10, 11, 12, 13, 14, 15
                                     });
    JMol mol;
    for (auto &alkane:alkanes) {
        mol.setAlkane(alkane);
        mol.randomize();
        MolItem molItem(mol);
        molItem.reloadHWData(0.1);
    }
    return 0;
}

int main() {
    try {
        return testHWDraw();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}