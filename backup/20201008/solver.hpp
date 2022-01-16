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
    static bool Solve(
            const std::vector<std::vector<T>> &in, std::vector<T> &out) {
        try {// 不搞越界判断和无解处理了，有问题直接抛异常返回false
            size_t rowSize = in.size(), colSize = in.at(0).size();
            // augmentedMatrix,增广矩阵
            std::vector<std::vector<T>> aMatrix(rowSize);
            for (auto &rMatrix:aMatrix) {
                rMatrix.resize(colSize);
            }
            for (size_t row = 0; row < rowSize; row++) {
                for (auto col = 0; col < colSize; col++) {
                    aMatrix[row][col] = in.at(row).at(col);
                }
            }
            size_t n = colSize - 1;// 未知数个数
            for (size_t i = 0; i < n; i++) {
                auto index = i;
                for (auto row = i + 1; row < rowSize; row++) {// 找到这一列最大的元素所在行
                    if (abs(aMatrix[row][i]) > abs(aMatrix[index][i])) {
                        index = row;
                    }
                }
//                show(index);
//                show(aMatrix);
                auto maxEleThisCol = aMatrix.at(index)[i];// 取对角线上的元素
                if (maxEleThisCol == 0) {
//                    std::cerr<<"index="<<index<<",i="<<i<<std::endl;
                    return false;
                }
                if (index != i) {// 做行交换，把最大的元素交换到当前对角线位置
                    swap(aMatrix[index], aMatrix[i]);
                }
                for (auto &ele:aMatrix[i]) {// 对角线元素归1,等比例处理这一行其它元素
                    ele /= maxEleThisCol;
                }// 用归一的对角线元素把该列其它所有元素归零
                for (size_t row = 0; row < rowSize; row++) {
                    if (row == i)continue;
                    auto k = aMatrix[row][i];
                    for (size_t col = i; col < colSize; col++) {
                        aMatrix[row][col] -= aMatrix[i][col] * k;
                    }
                }
            }
            std::vector<T> oMatrix(n);
            for (size_t i = 0; i < n; i++) {
                oMatrix[i] = aMatrix[i][colSize - 1];
            }
            auto den = oMatrix.at(0).denominator();
            for (auto &o:oMatrix) {
                den = lcm(den, o.denominator());
            }
            out.resize(n);
            for (size_t i = 0; i < n; i++) {
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
};

#endif //_LINEARSOLVER_H_
