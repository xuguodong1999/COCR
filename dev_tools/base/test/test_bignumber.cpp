#include <catch2/catch.hpp>
#include "base/bignumber.h"

TEST_CASE("bignumber 1", "add") {
    UnsignedInteger a{1}, b{2}, c{3};
    REQUIRE(a + b == c);
}

TEST_CASE("bignumber 2", "minus") {
    UnsignedInteger a{1}, b{2}, c{3};
    REQUIRE(c - b == a);
}