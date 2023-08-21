#include "text_assets.hpp"

#include "XGD/Base/Fraction.h"

#include "XGD/Chem/EquationBalanceSolver.h"
#include "XGD/Math/AlkaneGraph.h"
#include "XGD/Math/LinearSolver.h"
#include "XGD/Math/GreedyIsomerSolver.h"
#include "XGD/Math/PolyaIsomerSolver.h"

#include <boost/multiprecision/cpp_int.hpp>

#include <gtest/gtest.h>

#include <fstream>
#include <iostream>

TEST(fraction_1, add) {
    frac a{1}, b{2}, c{3};
    EXPECT_TRUE(a + b == c);
}

TEST(fraction_2, minus) {
    frac a{1}, b{2}, c{3};
    EXPECT_TRUE(c - b == a);
}

TEST(bignumber_1, add) {
    using UnsignedInteger = boost::multiprecision::cpp_int;
    UnsignedInteger a{1}, b{2}, c{3};
    EXPECT_TRUE(a + b == c);
}

TEST(bignumber_2, minus) {
    using UnsignedInteger = boost::multiprecision::cpp_int;
    UnsignedInteger a{1}, b{2}, c{3};
    EXPECT_TRUE(c - b == a);
}


TEST(balance_chemical_equations, loop) {
    auto lines = chemical_equations::CHEMICAL_EQUATIONS_ASSET_MAP.find("subset.txt");
    ASSERT_NE(lines, chemical_equations::CHEMICAL_EQUATIONS_ASSET_MAP.end());
    std::istringstream ifs(lines->second);
    EXPECT_TRUE(ifs.good());
    std::string line;
    while (std::getline(ifs, line) && !line.empty()) {
        auto result = CebApi::balance(line);
        if (!result) {
            FAIL() << line + " failed";
        } else {
            auto arr = result.value();
            EXPECT_TRUE(!arr.empty());
            auto html = CebApi::balanceAndRenderToHtml(line);
            EXPECT_TRUE(html) << html.value();
        }
    }
}


TEST(mol, hello_world) {
    EXPECT_TRUE(true);
}


TEST(PolyaIsomerCounter, count) {
    auto &counter = PolyaIsomerCounter::GetInstance();
    const auto &[c, a] = counter.count(4);
    EXPECT_TRUE(c == 2);
    EXPECT_TRUE(a == 4);
}


TEST(linear_solver, case1) {
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
    EXPECT_TRUE(out.size() == gt.size());
    for (size_t i = 0; i < out.size(); i++) {
        EXPECT_TRUE(out[i] == gt[i]);
    }
}

TEST(isomer, count) {
    const size_t target = 9;

    auto &counter1 = IsomerCounter::GetInstance();
    const auto result = counter1.calculate(target);

    auto &counter2 = PolyaIsomerCounter::GetInstance();
    const auto &[c, a] = counter2.count(target);

    EXPECT_TRUE(result != std::nullopt);
    EXPECT_TRUE(result.value() == c);
}

TEST(alkane_graph, case1) {
    AlkaneGraph<unsigned, unsigned> alkane1, alkane2;
    const std::string smi1{"CC(C)C"}, smi2{"C(C)CC"};
    alkane1.fromSMILES(smi1);
    alkane2.fromSMILES(smi2);
    EXPECT_TRUE(alkane1.hash() == alkane2.hash());
}
