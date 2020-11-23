//#include "torch_util.hpp"
//#include "torch_crnn.hpp"
#include "isomer.hpp"
#include <iostream>
#include <couch_data.hpp>
#include "mol.hpp"
#include "hw_mol.hpp"
using namespace std;

//void testCRNN() {
//    auto imgTensor = loadImgTensor("C:/Users/xgd/Pictures/crnn_test.png");
//    std::cout << "imgTensor.size=" << imgTensor.sizes() << std::endl;
//    auto crnn = std::make_shared<CRNN>(6947);
//    auto output = crnn->forward(imgTensor);
//    std::cout << "outSize=" << output.sizes() << std::endl;
//}

int testJMol() {
    // C1=CC=CC=C1
    CouchLoader::LoadCouchDataSet();
    auto &isomer = IsomerCounter::GetInstance();
    auto alkanes = isomer.getIsomers(
            {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18});
    for (auto &alkane:alkanes) {
        std::cout << "alkane=" << alkane << std::endl;
        JMol mol;
        mol.set("C1=CC=CC=C1C");//alkane);
        mol.randomize();
        mol.update2DCoordinates();
        MolItem molItem(mol);
        molItem.reloadHWData(0.5);
//        system("pause");
    }
    return 0;
}

int main() {
    try {
        return testJMol();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}