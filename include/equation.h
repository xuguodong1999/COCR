#ifndef _EQUATION_H_
#define _EQUATION_H_

#include "fraction.h"
#include "chemdata.h"
#include <iostream>
#include <mutex>
#include <thread>
#include <string>
#include <sstream>
#include <cassert>
#include <vector>
#include <map>
#include <list>
#include <unordered_map>

namespace cocr {
    class Equation {
    public:
//        static mutex mu;
//        static Equation *ep;
//        static Equation &getInstance() { // 锁代码段实现懒汉式单例类
//            if (ep) return *ep;
//            mu.lock();
//            if (!ep)
//                ep = new Equation();
//            mu.unlock();
//            return *ep;
//        }
        /**
         * @return 方程类单例
         * @brief 获得一个方程类单例
         * @example auto& equation=Equation::getInstance();
         */
        static Equation &getInstance() { // 函数内静态变量实现懒汉式单例类
            static Equation e;
            return e;
        }

        void test() {
            cout << "Equation::test" << endl;
            frac f1(3, 5), f2(5, 3), f3(0);
            auto s1 = f1.toString();
            frac f4;
            f4.fromString(s1);
            cout << f4 << endl;
            f4 += 1;
            cout << f4 << endl;
        }

        /**
         * @param equation 包含/不包含系数的化学方程式
         * @return 正确的系数
         * @brief 配平equation代表的化学方程式
         */
        vector<int> balance(const string &equation) {
            return vector<int>();
        }

        /**
         * @param reactants 反应物列表
         * @param products 生成物列表
         * @return 正确的系数
         * @brief 配平reactants->products代表的化学方程式
         */
        vector<int> balance(const vector<string> &reactants, const vector<string> &products) {
            return vector<int>();
        }

        /**
         * @param equation 包含系数的化学方程式
         * @return 是否正确配平
         * @brief 验证是否正确配平
         */
        bool isBalanced(const string &equation) {
        }

        /**
         * @param reactants 反应物列表
         * @param products 生成物列表
         * @param coefficient 需要验证的系数
         * @return 是否正确配平
         * @brief 验证是否正确配平
         */
        bool isBalanced(const vector<string> &reactants, const vector<string> &products,
                        const vector<int> &coefficient) {
        }

    private:
        Equation() {}

        ~Equation() {}

        Equation(const Equation &) = delete;

        const Equation &operator=(const Equation &) = delete;
    };
}
#endif //!_EQUATION_H_