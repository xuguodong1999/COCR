#include <iostream>
#include <torch/serialize.h>
#include <torch/cuda.h>
#include "timer.hpp"

using namespace std;

#include "torch_yolov4.hpp"
#include "torch_util.hpp"

int testYolov4() {
    torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);
//    torch::Device device(torch::kCPU);
    auto imgTensor = loadImgTensor("C:/Users/xgd/Pictures/cocr_test_1.png");
    std::cout << "imgTensor.size=" << imgTensor.sizes() << std::endl;
    auto model = std::make_shared<Yolov4>(17, 1.5);
    model->to(device);
    model->saveClassifier("D:/fuck");
    model->eval();
    Timer timer;
    timer.start();
    for (int i = 0; i < 40; i++) {
        auto output = model->forward(imgTensor.to(device));
        timer.display_duration();
    }
    torch::save(model, "D:/fuck.pth");
//    std::cout << "outSize=" << output.sizes() << std::endl;
    return 0;
}
//#include "torch_util.hpp"
//#include "torch_crnn.hpp"
//void testCRNN() {
//    auto imgTensor = loadImgTensor("C:/Users/xgd/Pictures/crnn_test.png");
//    std::cout << "imgTensor.size=" << imgTensor.sizes() << std::endl;
//    auto crnn = std::make_shared<CRNN>(6947);
//    auto output = crnn->forward(imgTensor);
//    std::cout << "outSize=" << output.sizes() << std::endl;
//}

//#include "mol_hw.hpp"
//#include "isomer.hpp"
//#include "couch_data.hpp"
//int testHWDraw() {
//    CouchLoader::LoadCouchDataSet(false);
//    auto &isomer = IsomerCounter::GetInstance();
//    auto alkanes = isomer.getIsomers({
//                                             3, 4, 5, 6, 7, 8, 10, 11, 12, 13, 14, 15
//                                     });
//    JMol mol;
//    for (auto &alkane:alkanes) {
//        mol.setAlkane(alkane);
//        mol.randomize();
//        MolHwItem molItem(mol);
//        molItem.reloadHWData(0.1);
//    }
//    return 0;
//}

//#include "darknet_data.hpp"
//int testDarknetDump(){
//    DarknetDataGenerator ddg;
//    ddg.init("G:/soso17");
//    ddg.dump(60000/3,3);
//    return 0;
//}
int main() {
    try {
        return testYolov4();
//        return testHWDraw();
//        return testDarknetDump();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}