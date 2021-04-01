#ifndef _XGD_COMPOSER_HANDLER_P_HPP_
#define _XGD_COMPOSER_HANDLER_P_HPP_

#include "jatom.hpp"
#include <vector>
#include <unordered_set>

namespace xgd {
    namespace _p {
        // 备注：以下，邻接电子对、化合价、邻接原子都表达中心原子键合状态的概念
        struct A {
            size_t id;
            float x, y, w, h;// 单元素图元的边框
            xgd::ElementType elementType;// 元素符号
            size_t curNebCount;// 该原子已经邻接的电子对数
            std::vector<size_t> nebs;// 邻居id，可能是键端or元素
            A(const size_t &_id, const float &_x, const float &_y, const float &_w, const float &_h,
              const xgd::ElementType &_type) : id(_id), x(_x), y(_y), w(_w), h(_h),
                                               elementType(_type), curNebCount(0) {}

        };

        struct G {// 基团
            size_t id;
            float x, y, w, h;// 水平字符串的边框
            // 拆解元素符号构造邻接电子对情况
            std::vector<std::pair<xgd::ElementType, int>> maxNebs;
            std::vector<std::pair<xgd::ElementType, int>> curNebs;
            std::vector<std::pair<size_t, size_t>> nebs;// <邻居id，基团内元素索引>，表明键端or原子应该和基团内哪一个元素符号结合

        };

        struct B;

        // 键端可能是隐式碳原子，也有可能和就近的元素符号结合
        struct BSide : public A {
            B *parent;// 键端所属键，用于约束和避免自环的问题
        public:
            BSide(const size_t &_id, const float &_x, const float &_y, B *p)
                    : parent(p), A(_id, _x, _y, 0, 0, xgd::ElementType::C) {}
        };

        struct B {
            size_t id;
            float x, y, w, h;
            BSide bs1, bs2;
        public:
            B(const size_t &_id, const size_t &_sid0, const size_t &_sid1,
              const float &_x0, const float &_y0, const float &_x1, const float &_y1)
                    : bs1(_sid0, _x0, _y0, this), bs2(_sid1, _x1, _y1, this),
                      id(_id), x((std::min)(_x0, _x1)), y((std::min)(_y0, _y1)),
                      w(std::fabs(_x0 - _x1)), h(std::fabs(_y0 - _y1)) {
            }
        };

        // 专指芳环中间的那个圈（强烈建议消灭这种写法 qwq
        struct C {
            size_t id;
            float x, y, w, h;// 这里用边框表示，期望在后处理中得到更精确的距离近似
            std::vector<size_t> nebs;// 邻居，可能是键端or元素
            size_t curNebCount;// 当前环绕的键图元数量
            C(const size_t &_id, const float &_x, const float &_y, const float &_w, const float &_h)
                    : id(_id), x(_x), y(_y), w(_w), h(_h), curNebCount(0) {}
        };
    }
}
#endif//_XGD_COMPOSER_HANDLER_P_HPP_
