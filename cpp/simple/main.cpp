#include <torch/torch.h>
#include "config.hpp"
using namespace std;
int main() {
    auto cuda_available = torch::cuda::is_available();
    torch::Device device(cuda_available ? torch::kCUDA : torch::kCPU);

    torch::Tensor scores = torch::rand({10,2,3});
    scores.print();
    std::tuple<torch::Tensor,torch::Tensor> sort_ret = torch::sort(scores.unsqueeze(1), 0, true);
    torch::Tensor v = std::get<0>(sort_ret).squeeze(1).to(scores.device());
    torch::Tensor idx = std::get<1>(sort_ret).squeeze(1).to(scores.device());
    std::cout<<scores<<std::endl;
    std::cout<<v<<std::endl;
    std::cout<<idx<<std::endl;


    return 0;
}
