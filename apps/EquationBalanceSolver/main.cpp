#include "XGD/Chem/EquationBalanceSolver.h"
#include <spdlog/spdlog.h>
#include <csignal>
#include <cstdlib>
#include <iostream>

int main() {
    spdlog::info(R"(press "Ctrl D" to quit...)");
    std::string input;
    spdlog::info("input: ...");
    while (std::getline(std::cin, input)) {
        auto result = CebApi::balanceAndRenderToHtml(input);
        if (result) {
            spdlog::info("output: {}", result.value());
        } else {
            spdlog::warn("failed, try another...");
        }
        spdlog::info("input: ...");
    }
    return EXIT_SUCCESS;
}
