#include <catch2/catch.hpp>
#include "math/polya.h"

TEST_CASE("PolyaIsomerCounter", "count") {
    auto &counter = PolyaIsomerCounter::GetInstance();
    const auto&[c, a]=counter.count(4);
    REQUIRE(c == 2);
    REQUIRE(a == 4);
}