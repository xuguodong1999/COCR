#include <catch2/catch.hpp>
#include <fstream>
#include "ceb/api.h"

const std::string TEST_FILE = TEST_SAMPLES_PATH + std::string("/datasets/easy-equations.txt");

TEST_CASE("balance chemical equations", "loop") {
    std::ifstream ifm(TEST_FILE);
    std::string line;
    while (std::getline(ifm, line)) {
        auto result = CebApi::balance(line);
        REQUIRE(result);
        auto arr = result.value();
        REQUIRE(!arr.empty());
    }
}