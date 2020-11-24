/**
 * This file is a part of COCR Project
 * @author 徐国栋
 */
#ifndef _STD_UTIL_HPP_
#define _STD_UTIL_HPP_

#include <string>
#include <vector>
#include <iostream>

std::string trim(const std::string &str);

std::string deleteSubStr(const std::string &_target, const std::string &_subStr);

std::string replaceSubStr(const std::string &_target,
                          const std::string &_subStr, const std::string &_newSubStr);

/**
 * @param _prob 返回值为真的概率
 * @return 概率为 _prob 的真值
 */
inline bool byProb(const float &_prob) {
    return rand() / static_cast<float>(RAND_MAX) < _prob;
}

/**
 * @param _prob
 * @return 返回 [0, _prob) 的浮点数
 */
inline float belowProb(const float &_prob) {
    return rand() / static_cast<float>(RAND_MAX) * _prob;
}

/**
 * @param _prob 取值 [0,1)
 * @return 返回 (1-_prob,1] 的浮点数
 */
inline float aboveProb(const float &_prob) {
    return 1.0 - rand() / static_cast<float>(RAND_MAX) * _prob;
}

/**
 * @param _posProb 一个正浮点数
 * @return (-_posProb,_posProb)
 */
inline float minMaxProb(const float &_posProb) {
    return _posProb - 2 * belowProb(_posProb);
}

template<typename T>
inline T &randSelect(std::vector<T> &_container) {
    if (_container.empty()) {
        std::cerr << "randSelect empty container" << std::endl;
        exit(-1);
    }
    return _container[rand() % _container.size()];
}

#endif//_STD_UTIL_HPP_
