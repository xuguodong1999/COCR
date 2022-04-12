#include <catch2/catch.hpp>
#include "math/hello.h"
using namespace els::math;

TEST_CASE("math::Hello", "add") {
    Hello hello;
    REQUIRE(hello.add(1, 1) == 1 + 1 + hello.getC());
}