#include "nn/util.h"
#include "nn/crnn.h"
#include <ATen/autocast_mode.h>
#include <iostream>

void testCRNN() {
    const auto device = torch::kCUDA;
    auto imgTensor = torch::zeros({102, 3, 192, 32}).to(device);
    std::cout << "imgTensor.size=" << imgTensor.sizes() << std::endl;
    auto crnn = std::make_shared<CRNN>(6947);
    crnn->to(device);
    while (true) {
        // mixed precision link amp
        at::autocast::set_enabled(true);
        crnn->forward(imgTensor);
        at::autocast::clear_cache();
        at::autocast::set_enabled(false);
    }
    auto output = crnn->forward(imgTensor);
    std::cout << "outSize=" << output.sizes() << std::endl;
}

int main() {
    testCRNN();
    return EXIT_SUCCESS;
}