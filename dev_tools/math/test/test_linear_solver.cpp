#include <catch2/catch.hpp>
#include "math/linear_solver.h"


TEST_CASE("linear_solver", "") {
    // x y z
    // 4 3 2
    // 2x+3y-4z=9
    // 5x+100y-8z=304
    // 21x-43y+7z=-31
    using arr = std::vector<frac>;
    using mat = std::vector<arr>;
    const mat in{
            {2,  3,   -4, 9},
            {5,  100, -8, 304},
            {21, -43, 7,  -31}
    };
    const arr out = linearSolve<frac>(in);
    const arr gt{4, 3, 2};
    REQUIRE(out.size() == gt.size());
    for (size_t i = 0; i < out.size(); i++) {
        REQUIRE(out[i] == gt[i]);
    }
}