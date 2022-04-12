#include <catch2/catch.hpp>
#include "ckit/hello.h"
using namespace els::ckit;

TEST_CASE("base::Hello", "add") {
    Hello hello;
    REQUIRE(hello.add(1, 1) == 1 + 1 + hello.getC());
}