#include "eigen_wrapper.h"
#include <iostream>

using namespace Eigen;
using namespace std;

int main() {
    MatrixXi m(9, 9);
    cout << m << endl;
    return EXIT_SUCCESS;
}