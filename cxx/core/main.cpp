#include "torch_util.hpp"
#include "torch_crnn.hpp"
#include <iostream>

using namespace std;

int _main() {

    auto imgTensor = loadImgTensor("C:/Users/xgd/Pictures/crnn_test.png");
    imgTensor.print();
    auto crnn = std::make_shared<CRNN>(80);
    auto output = crnn->forward(imgTensor);
    output.print();
    return 0;
}

int main() {
    try {
        return _main();
    } catch (std::exception &e) {
        std::cerr<<e.what()<<std::endl;
        return -1;
    }
}