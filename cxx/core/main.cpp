//#include "torch_util.hpp"
//#include "torch_crnn.hpp"

#include "mol.hpp"
#include "hw_mol.hpp"
#include "isomer.hpp"
#include "timer.hpp"
#include <iostream>
#include <couch_data.hpp>
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
            { 4,5,6,7,8,9,10});
    Timer timer;
    for (auto &alkane:alkanes) {
        std::cout << "alkane=" << alkane << std::endl;
        timer.start();
        JMol mol;
        mol.set(alkane);
        mol.randomize();
//        mol.set("C#CC");  // RDKit 内置2d引擎画的三键有问题
        std::cout<<"randomize="<<mol.toSMILES()<<std::endl;
        mol.update2DCoordinates();
        MolItem molItem(mol);
        timer.display_duration();
        molItem.reloadHWData(0.2);
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