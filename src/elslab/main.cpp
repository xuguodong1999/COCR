#include <torch/nn.h>
#include <torch/cuda.h>

int main() {
    torch::Device device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU);
    auto ones = torch::ones({4, 9}).to(device).to(torch::kFloat32);
    ones[3][8] = 4;
    std::cout << ones << std::endl;
    return 0;
}