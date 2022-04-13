#include <catch2/catch.hpp>
#include "base/fraction.h"

TEST_CASE("fraction 1", "add") {
    frac a{1}, b{2}, c{3};
    REQUIRE(a + b == c);
}

TEST_CASE("fraction 2", "minus") {
    frac a{1}, b{2}, c{3};
    REQUIRE(c - b == a);
}