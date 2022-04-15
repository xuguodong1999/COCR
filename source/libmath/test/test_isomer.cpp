#include <catch2/catch.hpp>
#include "math/isomer.h"
#include "math/polya.h"

TEST_CASE("isomer", "count") {
    const size_t target = 9;

    auto &counter1 = IsomerCounter::GetInstance();
    const auto result = counter1.calculate(target);

    auto &counter2 = PolyaIsomerCounter::GetInstance();
    const auto&[c, a]=counter2.count(target);

    REQUIRE(result != std::nullopt);
    REQUIRE(result.value() == c);
}