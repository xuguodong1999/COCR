#include <iostream>
#include <fdeep/fdeep.hpp>

int main() {
    auto vec = std::vector<uint8_t>({1, 2, 3, 4, 5, 6});
    auto tensor = fdeep::tensor_from_bytes(vec.data(), 2, 3, 1);
    std::cout << tensor.depth();
    return EXIT_SUCCESS;
}