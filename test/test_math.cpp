#include "gtest/gtest.h"
#include "fraction.h"
#include <vector>
#include <cmath>

template<typename T>
inline void test_fraction(const long long &add_to, const long long &mul_to) {
    using namespace std;
    vector<T> a(add_to, 0);
    T res = 0;
    for (int i = 0; i < add_to; i++) { a[i] = i; }
    for (int i = 0; i < add_to; i++) { res += i; }
    EXPECT_EQ(res, (0 + add_to - 1) * add_to / 2);
    res = 0;
    for (int i = 0; i < add_to; i++) { res += a[i]; }
    EXPECT_EQ(res, (0 + add_to - 1) * add_to / 2);
    res = 1;
    for (int i = 1; i < mul_to; i++) { res *= a[i]; }
    EXPECT_EQ(res, round(tgamma(mul_to)));
    res = 1;
    for (int i = 1; i < mul_to; i++) { res *= i; }
    EXPECT_EQ(res, round(tgamma(mul_to)));
}

TEST(FractionTestSuite, FractionTest) {
    test_fraction<short>(4, 4);
    test_fraction<int>(100, 10);
    test_fraction<long>(1000, 10);
    test_fraction<long long>(1000, 10);
}