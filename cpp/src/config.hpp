#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <iostream>
#include <memory>
#include <cmath>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif //_USE_MATH_DEFINES


template<typename T>
using vector = std::vector<T>;

template<typename T>
using function = std::function<T>;

template<typename T>
using shared_ptr = std::shared_ptr<T>;

using string = std::string;

using hash_type = uint64_t;
using node_type = unsigned char;

template<typename T>
inline T lcm(const T &x, const T &y) {
    return x * y / gcd(x, y);
}

inline const std::string &getDataDir() {
#ifdef WIN32
    static const std::string data_path = "C:/Users/xgd/source/COCR/data";
#else
    static const std::string data_path="/home/xgd/source/COCR/data";
#endif
    return data_path;
}

const std::string &getCouchFilePath();

const std::string &getDrugBankFilePath();

const std::string &getAlkaneCacheDir();

class Timer {
    decltype(std::chrono::system_clock::now())
            start_stamp, end_stamp, last_stamp;
public:
    void start();

    void display_duration();

    void stop(bool _display_duration = true);
};

std::string trim(const string &str);
extern const std::vector<string> ElementsData;
#endif//_CONFIG_HPP_
