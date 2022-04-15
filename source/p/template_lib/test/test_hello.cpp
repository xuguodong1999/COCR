#include <catch2/catch.hpp>
#include "base/hello.h"
using namespace els::base;

TEST_CASE("base::Hello", "add") {
    Hello hello;
    REQUIRE(hello.add(1, 1) == 1 + 1 + hello.getC());
}