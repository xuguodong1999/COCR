#pragma once

#include <cocr_base_export.h>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <random>

template<typename T>
inline std::string to_string_with_precision(const T _fValue, const int n = 2) {
    std::ostringstream out;
    out << std::setprecision(n) << _fValue;
    return out.str();
}

COCR_BASE_EXPORT std::string trim(const std::string &str);

COCR_BASE_EXPORT std::string deleteSubStr(const std::string &_target, const std::string &_subStr);

COCR_BASE_EXPORT std::string replaceSubStr(const std::string &_target,
                                           const std::string &_subStr, const std::string &_newSubStr);

COCR_BASE_EXPORT extern std::default_random_engine DRE;
COCR_BASE_EXPORT extern std::uniform_int_distribution<int> UID;

inline int randInt() {
    return UID(DRE);
}

/**
 * @param _prob 返回值为真的概率
 * @return 概率为 _prob 的真值
 */
inline bool byProb(const float &_prob) {
    return randInt() / static_cast<float>(RAND_MAX) < _prob;
}

/**
 * @param _prob
 * @return 返回 [0, _prob) 的浮点数
 */
inline float belowProb(const float &_prob) {
    return randInt() / static_cast<float>(RAND_MAX) * _prob;
}

/**
 * @param _prob 取值 [0,1)
 * @return 返回 (1-_prob,1] 的浮点数
 */
inline float aboveProb(const float &_prob) {
    return 1.0 - randInt() / static_cast<float>(RAND_MAX) * _prob;
}

inline float betweenProb(const float &_minProb, const float &_maxProb) {
    return _minProb + belowProb(_maxProb - _minProb);
}

/**
 * @param _posProb 一个正浮点数
 * @return (-_posProb,_posProb)
 */
inline float minMaxProb(const float &_posProb) {
    return _posProb - 2 * belowProb(_posProb);
}

/**
 * 返回容器内随机元素
 * @tparam T
 * @param _container
 * @return
 */
template<typename T>
inline const T &randSelect(const std::vector<T> &_container) {
    if (_container.empty()) {
        std::cerr << "randSelect empty container" << std::endl;
        exit(-1);
    }
    return _container[randInt() % _container.size()];
}

inline const char &randSelect(const std::string &_container) {
    if (_container.empty()) {
        std::cerr << "randSelect empty container" << std::endl;
        exit(-1);
    }
    return _container[randInt() % _container.size()];
}

template<typename T>
inline T getSum(const std::vector<T> &_container) {
    T t = 0;
    for (auto &c: _container) {
        t += c;
    }
    return std::move(t);
}

/**
 * 按照概率表返回容器内随机元素，要求容器大小和概率表大小相等
 * TODO: 降低时间复杂度
 * 计算策略：将原容器索引按照概率表映射到[0,RAND_MAX)，将[0,RAND_MAX)的随机数映射回索引
 * @tparam T
 * @param _container
 * @param _prob
 * @return
 */
template<typename T>
inline const T &randSelect(const std::vector<T> &_container, const std::vector<float> &_probMap) {
    if (_container.empty()) {
        std::cerr << "randSelect empty container" << std::endl;
        exit(-1);
    }
    if (_container.size() != _probMap.size()) {
        std::cerr << "randSelect container's size and probMap's size mismatch" << std::endl;
        exit(-1);
    }
    float sum = getSum(_probMap), cur = 0;
    sum *= belowProb(1.0);
    size_t i;
    for (i = 0; i < _probMap.size(); i++) {
        cur += _probMap[i];
        if (cur >= sum) {
            break;
        }
    }
    return _container[std::min(i, _container.size() - 1)];
}

inline std::string chopByFirstSpace(const std::string &_str) {
    auto pos = _str.find(" ");
    if (std::string::npos == pos) { return _str; }
    return _str.substr(0, pos);
}

struct pair_hash {
    template<class T1, class T2>
    inline std::size_t operator()(const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

template<typename E, typename T>
inline bool notExist(const T &_container, const E &_element) {
    return _container.end() == _container.find(_element);
}