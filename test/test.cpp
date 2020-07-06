#include "gtest/gtest.h"
#include "linearsolver.h"
#include "test.h"

TEST(LinearSolverTestSuite, LinearSolverTest) {
    using namespace cocr;
    auto &solver = LinearSolver::GetInstance();
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
}

TEST(ThirdPartyTestSuite, ThirdPartyTest) {
    EXPECT_EQ(helloEigen().size(), 2);
    EXPECT_EQ(helloOpenBabel().size() > 1, true);
    EXPECT_EQ(helloOpenCV() < 2000, true);
    EXPECT_EQ(helloRapidJson(), "{\"project\":\"rapidjson\",\"stars\":11}");
    EXPECT_EQ(helloQt(), "100");
}