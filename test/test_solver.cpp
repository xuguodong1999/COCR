#include "gtest/gtest.h"
#include "solver.h"
#include <vector>

TEST(LinearSolverTestSuite, LinearSolverTest) {
    using namespace cocr;
    auto &solver = LinearSolver::getInstance();
    vector<vector<int>> in = {{1, 1, 1,  6},
                              {1, 2, 2,  11},
                              {1, 4, -1, 6}};
    vector<int> out;
    bool ok = solver.solve(in, out);
    EXPECT_EQ(ok, true);
    EXPECT_EQ(out, vector<int>({1, 2, 3}));
    in = {{0, 2,  2, 0,  0,  -2, 0,  0},
          {1, 0,  0, -2, 0,  0,  0,  0},
          {1, 0,  0, 0,  -1, 0,  0,  0},
          {4, 2,  4, -4, -4, -1, -2, 0},
          {0, 0,  1, -1, -1, 0,  0,  0},
          {5, -2, 0, 0,  0,  0,  0,  0},   // 化合价因素
          {0, 0,  0, 0,  0,  0,  1,  1}    // 待定系数
    };
    ok = solver.solve(in, out);
    EXPECT_EQ(ok, true);
    EXPECT_EQ(out, vector<int>({2, 5, 3, 1, 2, 8, 5}));
    show(in);
    show(out);
}