#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>
#include "ceb/api.h"

const std::string TEST_FILE = DEV_ASSETS_DIR + std::string("/datasets/easy-equations.txt");

TEST_CASE("balance chemical equations", "loop") {
    std::ifstream ifs(TEST_FILE, std::ios_base::in);
    REQUIRE(ifs.good());
    std::string line;
    while (std::getline(ifs, line) && !line.empty()) {
        auto result = CebApi::balance(line);
        if (!result) {
            INFO(line + " failed");
            FAIL();
        } else {
            auto arr = result.value();
            REQUIRE(!arr.empty());
            auto html = CebApi::balanceAndRenderToHtml(line);
            INFO(html.value());
            REQUIRE(html);
        }
    }
    ifs.close();
}