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
    using namespace std;



    class Equation {
    public:
//        static mutex mu;
//        static Equation *ep;
//        static Equation &GetInstance() { // 锁代码段实现懒汉式单例类
//            if (ep) return *ep;
//            mu.lock();
//            if (!ep)
//                ep = new Equation();
//            mu.unlock();
//            return *ep;
//        }
        static Equation &GetInstance() { // 函数内静态变量实现懒汉式单例类
            static Equation e;
            return e;
        }

        void test() {
            cout << "Equation::test" << endl;
            Fraction f1(3, 5), f2(5, 3), f3(0);
            auto s1 = f1.toString();
            Fraction f4;
            f4.fromString(s1);
            cout << f4 << endl;
            f4 += 1;
            cout << f4 << endl;
        }

    private:
        Equation() {}

        ~Equation() {}

        Equation(const Equation &) = delete;

        const Equation &operator=(const Equation &) = delete;
    };
}
#endif //!_EQUATION_H_