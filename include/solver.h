#ifndef _LINEARSOLVER_H_
#define _LINEARSOLVER_H_

#include "cocr.h"
#include "fraction.h"
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>

namespace cocr {
    using namespace std;

    class LinearSolver {
    public:
        /**
         * @return 线性求解类单例
         * @brief 获得一个线性求解类单例
         * @example auto& ls=LinearSolver::getInstance();
         */
        static LinearSolver &getInstance() {
            static LinearSolver ls;
            return ls;
        }

        /**
         * @param in 增广矩阵
         * @param out 一组解
         * @return 能否找到一组解
         * @brief 以分数形式求解增广矩阵表达的线性方程组
         */
        template<typename T>
        bool solve(const vector<vector<T>> &in, vector<T> &out) {
            try {// 不搞越界判断和无解处理了，有可能出问题的地方用at，直接抛异常返回false
                auto rowSize = in.size(), colSize = in.at(0).size();
                vector<vector<frac>> aMatrix(rowSize);// augmentedMatrix,增广矩阵
                for (auto &rMatrix:aMatrix) {
                    rMatrix.resize(colSize);
                }
                for (auto row = 0; row < rowSize; row++) {
                    for (auto col = 0; col < colSize; col++) {
                        aMatrix[row][col] = in.at(row).at(col);
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
                    auto maxEleThisCol = aMatrix.at(index)[i];// 取对角线上的元素
                    if (maxEleThisCol == 0) {
                        return false;
                    }
                    if (index != i) {// 做行交换，把最大的元素交换到当前对角线位置
                        swap(aMatrix[index], aMatrix[i]);
                    }
                    for (auto &ele:aMatrix[i]) {// 对角线元素归1,等比例处理这一行其它元素
                        ele /= maxEleThisCol;
                    }// 用归一的对角线元素把该列其它所有元素归零
                    for (auto row = 0; row < rowSize; row++) {
                        if (row == i)continue;
                        auto k = aMatrix[row][i];
                        for (auto col = i; col < colSize; col++) {
                            aMatrix[row][col] -= aMatrix[i][col] * k;
                        }
                    }
//                    show(aMatrix);
                }
                vector<frac> oMatrix(n);
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
            } catch (out_of_range e) {
                show(e.what());
                return false;
            } catch (exception e) {
                show(e.what());
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
