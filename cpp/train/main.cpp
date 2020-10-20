#include "config.hpp"

#include <string>
#include <exception>
#include <iostream>
#include <iomanip>

void demo_torch();

void demo_opencv();

std::string demo_openbabel(const char *smi = "c1ccccc1");

void demoSingle();

void demoLine();

void demoCircle();

void demoSymbol();

void demoRichSymbol();

void demoSpecialSymbol();

void demoBond();

void makeIcon();

void demoMol();

void loop(f<void(void)> demoFunc) {
    srand(918);
    while (true) { demoFunc(); }
}

void trainSimpleClassifier();

void testSwish();

inline void put_env() {
#ifdef WIN32
    _putenv(const_cast<char *>("BABEL_DATADIR=C:/static/openbabel-3.1.1/data"));
#elif defined(unix)
    putenv(const_cast<char*>("BABEL_DATADIR=/home/xgd/install/openbabel/share/openbabel/3.1.0"));
#endif
}

#include "isomer.hpp"
#include "graph.hpp"
#include <sstream>

template<typename T>
inline void test_graph() {
    using namespace std;
    using number = T;
    stringstream input("0 1 "
                       "2 1 "
                       "2 3 "
                       "3 4 "
                       "4 5 ");
    UGraph<number> ut;
    input >> ut;
    std::cout << ut.toString() << std::endl;
    exit(-1);
    set<number> nodes;
    auto simpleLogTraverse = [&](Graph<number> &g, const number &current, const number &from) {
        cout << (long long) current << " is visited! My father node is "
             << (long long) from << " and here are my neighbors: [";
        // 节点只被处理一次
        std::cout << "gtest::EXPECT_EQ=" << (nodes.find(current) == nodes.end()) << std::endl;
        nodes.insert(current);
        for (auto &neighbor:g.getData()[current]) {
            cout << (long long) neighbor << ",";
        }
        cout << "]" << endl;
    };
    cout << "dfs" << endl;
    nodes.clear();
    ut.dfsWrapper(simpleLogTraverse, ut.idle, ut.idle);
//    EXPECT_EQ(ut.getNodeNum(), nodes.size());
    cout << "bfs" << endl;
    nodes.clear();
    ut.bfsWrapper(simpleLogTraverse, ut.idle, ut.idle);
//    EXPECT_EQ(ut.getNodeNum(), nodes.size());
}

int main(int argc, char **argv) {
    put_env();
//    demo_opencv();
//    demo_torch();
//    std::cout<<demo_openbabel()<<std::endl;
//    loop(demoMol);
//    demoMol();

//    trainSimpleClassifier();
//    auto &ic = IsomerCounter::getInstance();
//    ic.calculate(1);
    test_graph<int>();
    return 0;
}
