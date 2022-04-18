#include "test_one_hot_vector.h"
#include "../facilities/calculate_tags.h"
#include <iostream>
#include <cassert>
#include <MetaNN/meta_nn.h>

using namespace std;
using namespace MetaNN;

namespace {
    void test_one_hot_vector1() {
        cout << "Test one-hot vector case 1...\t";
        static_assert(IsMatrix<OneHotVector<int, CheckDevice>>, "Test Error");
        static_assert(IsMatrix<OneHotVector<int, CheckDevice> &>, "Test Error");
        static_assert(IsMatrix<OneHotVector<int, CheckDevice> &&>, "Test Error");
        static_assert(IsMatrix<const OneHotVector<int, CheckDevice> &>, "Test Error");
        static_assert(IsMatrix<const OneHotVector<int, CheckDevice> &&>, "Test Error");

        auto rm = OneHotVector<int, CheckDevice>(100, 37);
        REQUIRE(rm.RowNum() == 1);
        REQUIRE(rm.ColNum() == 100);
        REQUIRE(rm.HotPos() == 37);

        auto rm1 = Evaluate(rm);
        for (size_t i = 0; i < 1; ++i) {
            for (size_t j = 0; j < 100; ++j) {
                if (j != 37) {
                    REQUIRE(rm1(i, j) == 0);
                } else {
                    REQUIRE(rm1(i, j) == 1);
                }
            }
        }

        cout << "done" << endl;
    }

    void test_one_hot_vector2() {
        cout << "Test one-hot vector case 2...\t";
        auto rm1 = OneHotVector<int, CheckDevice>(100, 37);
        auto rm2 = OneHotVector<int, CheckDevice>(50, 16);

        auto evalRes1 = rm1.EvalRegister();
        auto evalRes2 = rm2.EvalRegister();

        EvalPlan<DeviceTags::CPU>::Eval();
        for (size_t j = 0; j < 100; ++j) {
            if (j == 37) {
                REQUIRE(evalRes1.Data()(0, j) == 1);
            } else {
                REQUIRE(evalRes1.Data()(0, j) == 0);
            }
        }

        for (size_t j = 0; j < 50; ++j) {
            if (j == 16) {
                REQUIRE(evalRes2.Data()(0, j) == 1);
            } else {
                REQUIRE(evalRes2.Data()(0, j) == 0);
            }
        }
        cout << "done" << endl;
    }
}

void test_one_hot_vector() {
    test_one_hot_vector1();
    test_one_hot_vector2();
}
