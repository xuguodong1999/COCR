//
// Created by xgd on 2020/10/3.
//

#ifndef COCR_SOSO918_HPP
#define COCR_SOSO918_HPP
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
inline T gcd(const T &x, const T &y) {
    return x % y == 0 ? y : gcd(y, x % y);
}

template<typename T>
inline T lcm(const T &x, const T &y) {
    return x * y / gcd(x, y);
}

#endif //COCR_SOSO918_HPP
