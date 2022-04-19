#include <torch/nn/init.h>

int main(int argc, char **argv) {
    auto input = torch::ones({3, 4});
    auto input1 = torch::ones({3, 4});
    std::cout << input.sizes();
    auto catted = torch::cat({input, input1}, 0);
    std::cout << catted.sizes();
    auto vec = catted.split(input.size(0));
    std::cout << vec.size();
    std::cout << vec[0].sizes();
    std::cout << vec[1].sizes();
    return EXIT_SUCCESS;
}