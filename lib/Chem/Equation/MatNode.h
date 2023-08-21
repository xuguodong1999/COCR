#pragma once

#include "Element.h"
#include <vector>

class MatNode {
public:
    bool isNum;
    bool isHead; //  用于建立语法树以及下标显示 控制台程序用不到
    int num;
    std::vector<Element> block; //  block 代表元素阵列 记录一个matnode里面的所有元素
    MatNode();

    ~MatNode();

    void clear();

    void compress(); //  压缩元素阵列 合并相同的元素 累加元素数量
    void print();
};
