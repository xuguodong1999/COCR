#define TEST_ALL_LIB_WORK
#ifdef TEST_ALL_LIB_WORK

//#include "test.cpp"
#include "equation.h"
#include "linearsolver.h"
#include <QApplication>
#include <exception>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
//    QApplication a(argc, argv);
//    try {
//        helloEigen();
//        helloOpenBabel();
//        helloOpenCV();
//        helloQt();
//        helloRapidJson();
//    } catch (std::exception e) {
//        std::cout << e.what() << std::endl;
//    }
//    return a.exec();
    using namespace cocr;
    using namespace std;
    auto &e = Equation::GetInstance();
    e.test();
    auto &cd = ChemistryData::GetInstance();
    cout << cd.mass.size() << endl
         << cd.electronegativity.size() << endl
         << cd.name.size() << endl;
//    for (const string &s:cd.valence) {
//        if (s.find(" ") != string::npos) {
//
//        } else {
//            cout<<"},";
//        }
//    }
    auto &solver = LinearSolver::GetInstance();
    vector<vector<int>> in = {
            {1, 1, 1,  6},
            {1, 2, 2,  11},
            {1, 4, -1, 6}
    };
    vector<int> out;
    solver.solve(in, out);
    for (auto &ele:out) {
        cout << ele << ",";
    }
    cout << endl;
    return 0;
}

#endif //!TEST_1