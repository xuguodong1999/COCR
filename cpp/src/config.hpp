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

const char *get_couch_data_path();

const char *get_coordinates_json_path(int id = 0);

class Timer {
    decltype(std::chrono::system_clock::now())
            start_stamp, end_stamp, last_stamp;
public:
    void start();

    void display_duration();

    void stop(bool _display_duration = true);
};

std::string trim(const string &str);

#endif//_CONFIG_HPP_
