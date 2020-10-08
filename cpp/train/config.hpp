#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include <vector>
#include <string>
#include <list>
#include <functional>
#include <iostream>
#include <memory>
#include <cmath>

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif //_USE_MATH_DEFINES


template<typename T>
using v = std::vector<T>;

template<typename T>
using l = std::list<T>;

template<typename T>
using f = std::function<T>;

template<typename T>
using p = std::shared_ptr<T>;

using s = std::string;

template<typename T>
inline T lcm(const T &x, const T &y) {
    return x * y / gcd(x, y);
}

const char *get_couch_data_path();

const char *get_coordinates_json_path(int id = 0);

#endif//_CONFIG_HPP_
