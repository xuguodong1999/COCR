#ifndef _COCR_H_
#define _COCR_H_

#include <vector>
#include <iostream>
#include <list>

#define LogName(value) std::cout << (#value) << "=" << (value) << std::endl;
#define LogLine(value) std::cout << (value) << std::endl;
#define LogValue(value) std::cout << (value);
#define LogPressKey2Continue() std::cout << std::endl << "press any key to continue..." << std::endl;
namespace cocr {
    template<typename T>
    inline T mod32(const T &t) {
        return t - t % 32;
    }

    template<typename T>
    inline void show(const T &t, std::ostream &out = std::cout) {
        out << t << std::endl;
    }

    template<typename T>
    inline void show(const std::vector<std::vector<T>> &arr2d, std::ostream &out = std::cout) {
        out << "[" << std::endl;
        for (auto &arr : arr2d) {
            out << " [";
            for (auto &ele : arr) {
                out << ele << ",";
            }
            out << "]" << std::endl;
        }
        out << "]" << std::endl;
    }

    template<typename T>
    inline void show(const std::vector<T> &arr, std::ostream &out = std::cout) {
        out << "[";
        for (auto &ele : arr) {
            out << ele << ",";
        }
        out << "]" << std::endl;
    }

    template<typename T>
    inline void show(const std::list<T> &arr, std::ostream &out = std::cout) {
        out << "[";
        for (auto &ele : arr) {
            out << ele << ",";
        }
        out << "]" << std::endl;
    }
}
#endif
