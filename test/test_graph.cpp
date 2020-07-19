#include "gtest/gtest.h"
#include "graph.h"
#include <set>
#include <sstream>

template<typename T>
inline void test_graph() {
    using namespace std;
    typedef T number;
    stringstream input("6 1 "
                       "5 1 "
                       "2 1 "
                       "3 1 "
                       "7 2 "
                       "4 2 "
                       "0 3 "
                       "0 4 ");
    cocr::UGraph<number> ut;
    input >> ut;
    set<number> nodes;
    auto simpleLogTraverse = [&](cocr::Graph<number> &g, const number &current, const number &from) {
        cout << (long long) current << " is visited! My father node is "
             << (long long) from << " and here are my neighbors: [";
        EXPECT_EQ(nodes.find(current), nodes.end());// 节点只被处理一次
        nodes.insert(current);
        for (auto &neighbor:g.getData()[current]) {
            cout << (long long) neighbor << ",";
        }
        cout << "]" << endl;
    };
    cout << "dfs" << endl;
    nodes.clear();
    ut.dfsWrapper(simpleLogTraverse, ut.idle, ut.idle);
    EXPECT_EQ(ut.getNodeNum(), nodes.size());
    cout << "bfs" << endl;
    nodes.clear();
    ut.bfsWrapper(simpleLogTraverse, ut.idle, ut.idle);
    EXPECT_EQ(ut.getNodeNum(), nodes.size());
}

TEST(GraphTestSuite, GraphTest) {
    test_graph<unsigned char>();
    test_graph<unsigned short>();
    test_graph<unsigned>();
    test_graph<unsigned long>();
    test_graph<unsigned long long>();
    test_graph<short>();
    test_graph<int>();
    test_graph<long>();
    test_graph<long long>();
}