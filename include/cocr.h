#ifndef _COCR_H_
#define _COCR_H_

#include <vector>
#include <iostream>
#include <list>

namespace cocr {
    using namespace std;

    template<typename T>
    inline void show(const T &t, ostream &out = cout) {
        out << t << endl;
    }

    template<typename T>
    inline void show(const vector<vector<T>> &arr2d, ostream &out = cout) {
        out << "[" << endl;
        for (auto &arr : arr2d) {
            out << " [";
            for (auto &ele : arr) {
                out << ele << ",";
            }
            out << "]" << endl;
        }
        out << "]" << endl;
    }

    template<typename T>
    inline void show(const vector<T> &arr, ostream &out = cout) {
        out << "[";
        for (auto &ele : arr) {
            out << ele << ",";
        }
        out << "]" << endl;
    }
}
#endif
