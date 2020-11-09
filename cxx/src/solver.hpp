#ifndef _SOLVER_HPP_
#define  _SOLVER_HPP_

#include "fraction.hpp"
#include <vector>
#include <iostream>
#include <cassert>
#include <algorithm>

template<typename T>
inline T lcm(const T &x, const T &y) {
    return x * y / gcd(x, y);
}

/**
 * @param in 增广矩阵
 * @return out 一组解
 * @brief 以分数形式求解增广矩阵表达的线性方程组
 */
template<typename T>
inline std::vector<T> linearSolve(const std::vector<std::vector<T>> &in) {
    std::vector<T> out;
    size_t rowSize = in.size();
    if (rowSize < 1) { return std::move(out); }
    size_t colSize = in.at(0).size();
    if (colSize < 2) { return std::move(out); }
    std::vector<std::vector<T>> aMatrix(rowSize);
    for (auto &rMatrix:aMatrix) {
        rMatrix.resize(colSize);
    }
    for (size_t row = 0; row < rowSize; row++) {
        if (in[row].size() != colSize) { return std::move(out); }
        for (auto col = 0; col < colSize; col++) {
            aMatrix[row][col] = in[row][col];
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
        for (size_t row = 0; row < rowSize; row++) {
            if (row == i)continue;
            auto k = aMatrix[row][i];
            for (size_t col = i; col < colSize; col++) {
                aMatrix[row][col] -= aMatrix[i][col] * k;
            }
        }
    }
    out.resize(n);
    for (size_t i = 0; i < n; i++) {
        out[i] = aMatrix[i][colSize - 1];
    }
    auto den = out.at(0).denominator();
    for (auto &o:out) {
        den = lcm(den, o.denominator());
    }
    for (size_t i = 0; i < n; i++) {
        out[i] *= den;
    }
    return std::move(out);
}

#endif// _SOLVER_HPP_