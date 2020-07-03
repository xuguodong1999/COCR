#ifndef _LINEARSOLVER_H_
#define _LINEARSOLVER_H_

#include "fraction.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>

#define MAXM 50
namespace cocr {
    using namespace std;

    class LinearSolver {
    public:
        static LinearSolver &GetInstance() {
            static LinearSolver ls;
            return ls;
        }

        bool solve(const vector<vector<int>> &in, vector<int> &out) {
            try {// 不搞越界判断和无解处理了，有可能出问题的地方用at，直接抛异常返回false
                auto rowSize = in.size(), colSize = in.at(0).size();
//                cout << "rowSize=" << rowSize << ",colSize=" << colSize << endl;
                vector<vector<Fraction>> aMatrix(rowSize);// augmentedMatrix,增广矩阵
                for (auto &rMatrix:aMatrix) {
                    rMatrix.resize(colSize);
                }
                for (auto row = 0; row < rowSize; row++) {
                    assert(in.at(row).size() == colSize);
                    for (auto col = 0; col < colSize; col++) {
                        aMatrix[row][col] = in[row][col];
                    }
                }
                auto n = colSize - 1;// 未知数个数
                for (auto i = 0; i < n; i++) {
                    auto index = i;
                    for (auto row = i + 1; row < rowSize; row++) {// 找到这一列最大的元素所在行
                        if (aMatrix[row][i] > aMatrix[index][i]) {
                            index = row;
                        }
                    }
                    auto maxEleThisCol = aMatrix[index][i];// 取对角线上的元素
//                    cout << "max=" << maxEleThisCol << endl;
                    if (maxEleThisCol == 0) {
                        return false;
                    }
                    if (index != i) {// 做行交换，把最大的元素交换到当前对角线位置
//                        cout << "swap " << index << " and " << i << endl;
                        swap(aMatrix.at(index), aMatrix.at(i));
                    }
                    for (auto &ele:aMatrix.at(i)) {// 对角线元素归1,等比例处理这一行其它元素
                        ele /= maxEleThisCol;
                    }// 用归一的对角线元素把该列其它所有元素归零
                    for (auto row = 0; row < rowSize; row++) {
                        if (row == i)continue;
                        auto k = aMatrix[row][i];
                        for (auto col = i; col < colSize; col++) {
                            aMatrix[row][col] -= aMatrix[i][col] * k;
                        }
                    }
//                    for(auto&r:aMatrix){
//                        for(auto&c:r){
//                            cout<<c<<", ";
//                        }
//                        cout<<endl;
//                    }
                }
                vector<Fraction> oMatrix;
                oMatrix.resize(n);
                for (auto i = 0; i < n; i++) {
                    oMatrix[i] = aMatrix[i][colSize - 1];
                }
                auto den = oMatrix.at(0).denominator();
                for (auto &o:oMatrix) {
                    den = lcm(den, o.denominator());
                }
                out.resize(n);
                for (auto i = 0; i < n; i++) {
                    out[i] = oMatrix[i].numerator() * den / oMatrix[i].denominator();
                }
            } catch (exception e) {
//                cout << e.what() << endl;
                return false;
            }
            return true;
        }

    private:
        LinearSolver() {}

        ~LinearSolver() {}

        LinearSolver(const LinearSolver &) = delete;

        const LinearSolver &operator=(const LinearSolver &) = delete;

        int gcd(int x, int y) const {
            return x % y == 0 ? y : gcd(y, x % y);
        }

        int lcm(int x, int y) const {
            return x * y / gcd(x, y);
        }
    };
}
#endif //_LINEARSOLVER_H_
