#include "mobilenetv3.hpp"
#include <torch/torch.h>
#include <fstream>
#include<chrono>
#include<thread>

int main() {
    auto cuda_available = torch::cuda::is_available();
    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);
    auto net = std::make_shared<MobileNetV3>(4096);
    net->to(device);
    auto test_img = torch::rand({10, 3, 416, 416}).to(device);
    while (1) {
//        std::cout << test_img.sizes() << std::endl;
        auto test_out = net->forward(test_img);
//        std::cout << test_out.sizes() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    }
    return 0;
}

void test_() {
    //    s model_path = "D:/mv3.pt";
    // way 1 2 work between cpp but error from cpp to python
    // way 1
//    torch::serialize::OutputArchive output_archive;
//    net->save(output_archive);
//    output_archive.save_to(model_path);
    // way 2
//    torch::save(net, model_path);

    // way 3
//    std::ofstream ofsm(model_path,std::ios::binary);
//    for(auto&layer:net->named_parameters()){
//        std::cout<<layer.key()<<std::endl;
//        auto dat=torch::pickle_save(layer.value());
//        ofsm.write(dat.data(),dat.size());
//    }
//    ofsm.close();
}