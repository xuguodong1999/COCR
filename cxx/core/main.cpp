/**
 * author: xgd
 */
#include "timer.hpp"
#include "torch_util.hpp"
#include "torch_mv3.hpp"
#include <torch/torch.h>

extern std::string WEIGHTS_DIR;

int main() {
    torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);
    auto model = std::make_shared<Mv3Large>(6946);
    model->setName("gb-6946");
    model->to(device);
//    torch::load(model, WEIGHTS_DIR + "/gb_classification/mv3-epoch-0.pth", device);
//    model->saveClassifier("D:/","in","bneck","out");
    model->loadClassifier("D:/","in","bneck","out",device);
    model->freezeOutLayer(true);
    model->eval();
    Timer timer;
    timer.start();
    auto input = loadImgTensor("C:/Users/xgd/source/COCR/cache/simple_test/liang.png");
    auto output = model->forward(input.to(device));
    int topk = 5;
    auto prediction = output.topk(topk, 1);
    std::cout << std::get<0>(prediction).squeeze() << std::endl;
    auto indices = std::get<1>(prediction).squeeze(0);
    for (int i = 0; i < topk; i++) {
        std::cout << indices[i].item().toInt() << std::endl;
    }
    timer.stop();
    output.print();
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return 0;
}