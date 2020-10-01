#ifndef _LINEARSOLVER_H_
#define _LINEARSOLVER_H_

#include "fraction.hpp"
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>


class LinearSolver {
public:
    /**
     * @param in 增广矩阵
     * @param out 一组解
     * @return 能否找到一组解
     * @brief 以分数形式求解增广矩阵表达的线性方程组
     */
    template<typename T>
    static bool solve(
            const std::vector<std::vector<T>> &in, std::vector<T> &out) {
        try {// 不搞越界判断和无解处理了，有问题直接抛异常返回false
            auto rowSize = in.size(), colSize = in.at(0).size();
            std::vector<std::vector<frac>> aMatrix(rowSize);// augmentedMatrix,增广矩阵
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
            }
            std::vector<frac> oMatrix(n);
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
            std::cerr << e.what() << std::endl;
            return false;
        } catch (exception e) {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

private:
    LinearSolver() {}

    ~LinearSolver() {}

    LinearSolver(const LinearSolver &) = delete;

    const LinearSolver &operator=(const LinearSolver &) = delete;

    static int gcd(int x, int y) {
        return x % y == 0 ? y : gcd(y, x % y);
    }

    static int lcm(int x, int y) {
        return x * y / gcd(x, y);
    }
};

#endif //_LINEARSOLVER_H_
