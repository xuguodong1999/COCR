#include "mobilenetv3.hpp"
#include <torch/torch.h>
#include <fstream>

void trainSimpleClassifier() {
    auto cuda_available = torch::cuda::is_available();
    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);
    auto net = std::make_shared<MobileNetV3>(10);
    net->to(device);
//    torch::load(net,"/tmp/mv3.pt");
    auto test_img = torch::rand({2, 3, 640, 416}).to(device);
    std::cout << test_img.sizes() << std::endl;
    // try {
        while(1){
        auto test_out = net->forward(test_img);

        std::cout << test_out.sizes() << std::endl;
        }
    // } catch (std::exception e) {
        // std::cerr << e.what() << std::endl;
    // }
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