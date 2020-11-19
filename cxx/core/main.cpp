//#include "torch_util.hpp"
//#include "torch_crnn.hpp"
#include <iostream>
#include "mol.hpp"
#include "generator.hpp"

using namespace std;

//void testCRNN() {
//    auto imgTensor = loadImgTensor("C:/Users/xgd/Pictures/crnn_test.png");
//    std::cout << "imgTensor.size=" << imgTensor.sizes() << std::endl;
//    auto crnn = std::make_shared<CRNN>(6947);
//    auto output = crnn->forward(imgTensor);
//    std::cout << "outSize=" << output.sizes() << std::endl;
//}

int testJMol() {
    JMol mol;
    mol.set("C1=CC=CC=C1");
    mol.update2DCoordinates();
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