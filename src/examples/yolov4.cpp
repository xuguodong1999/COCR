#include "base/timer.h"
#include "nn/yolov4.h"

#include <torch/serialize.h>
#include <torch/cuda.h>

#include <iostream>

using namespace std;

int testYolov4() {
    torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);
    auto imgTensor = torch::zeros({1, 3, 32, 32});
    std::cout << "imgTensor.size=" << imgTensor.sizes() << std::endl;
    auto model = std::make_shared<Yolov4>(17, 1.5);
    model->to(device);
    model->saveClassifier("/tmp/classifier.pth");
    model->eval();
    Timer timer;
    timer.start();
    for (int i = 0; i < 40; i++) {
        auto output = model->forward(imgTensor.to(device));
        timer.display_duration();
    }
    torch::save(model, "/tmp/yolov4.pth");
    return 0;
}

int main() {
    testYolov4();
    return EXIT_SUCCESS;
}