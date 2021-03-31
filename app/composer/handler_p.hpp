#ifndef _XGD_COMPOSER_HANDLER_P_HPP_
#define _XGD_COMPOSER_HANDLER_P_HPP_

#include "jatom.hpp"
#include <vector>

namespace _p {
    // 备注：以下，邻接电子对、化合价、邻接原子都表达中心原子键合状态的概念
    class A {
    protected:
        size_t id;
        float x, y, w, h;// 单元素图元的边框
        xgd::ElementType elementType;// 元素符号
        size_t maxNebCount;// 该元素符号对应原子一般能邻接多少原子，参考化合价作经验设置
        size_t curNebCount;// 该原子已经邻接的电子对数
        std::vector<size_t> nebs;// 邻居id，可能是键端or元素
    };

    class G {// 基团
        size_t id;
        float x, y, w, h;// 水平字符串的边框
        // 拆解元素符号构造邻接电子对情况
        std::vector<std::pair<xgd::ElementType, int>> maxNebs;
        std::vector<std::pair<xgd::ElementType, int>> curNebs;
        std::vector<std::pair<size_t, size_t>> nebs;// <邻居id，基团内元素索引>，表明键端or原子应该和基团内哪一个元素符号结合
    };

    class B;

    // 键端可能是隐式碳原子，也有可能和就近的元素符号结合
    class BSide : public A {
        B *parent;// 键端所属键，用于约束和避免自环的问题
    public:
        BSide(B *p) : parent(p) {}
    };

    class B {
        size_t id;
        float x, y, w, h;
        BSide bs1, bs2;
        int count;
    public:
        B() : bs1(this), bs2(this) {}
    };

    // 专指芳环中间的那个圈（强烈建议消灭这种写法 qwq
    class C {
        size_t id;
        float x, y, w, h;// 这里用边框表示，期望在后处理中得到更精确的距离近似
        std::vector<size_t> nebs;// 邻居，可能是键端or元素
        size_t minNebCount;// 至少设为3来避免自环
        size_t curNebCount;// 当前环绕的键图元数量
    };
}
#endif//_XGD_COMPOSER_HANDLER_P_HPP_
