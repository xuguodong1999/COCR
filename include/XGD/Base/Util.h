#pragma once

#include "xgd_base_export.h"

#include "XGD/SpdlogStream/Stream.h"

#include <iostream>
#include <iosfwd>
#include <filesystem>
#include <functional>
#include <unordered_set>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <random>

class XGD_BASE_EXPORT StdUtil {
    inline static std::default_random_engine DRE{171860633};
    inline static std::uniform_int_distribution<int> UID{0, RAND_MAX};
public:
    template<typename T>
    static std::string to_string_with_precision(const T _fValue, const int n = 2) {
        std::ostringstream out;
        out << std::setprecision(n) << _fValue;
        return out.str();
    }

    static bool byProb(const float &_prob) {
        return randInt() / static_cast<float>(RAND_MAX) < _prob;
    }

    static int randInt() {
        return UID(DRE);
    }

    static float belowProb(const float &_prob) {
        return randInt() / static_cast<float>(RAND_MAX) * _prob;
    }

    static float aboveProb(const float &_prob) {
        return 1.0 - randInt() / static_cast<float>(RAND_MAX) * _prob;
    }

    static float betweenProb(const float &_minProb, const float &_maxProb) {
        return _minProb + belowProb(_maxProb - _minProb);
    }

    static float minMaxProb(const float &_posProb) {
        return _posProb - 2 * belowProb(_posProb);
    }

    template<typename T>
    static const T &randSelect(const std::vector<T> &_container) {
        if (_container.empty()) {
            xgd::serr << "randSelect empty container" << std::endl;
            exit(-1);
        }
        return _container[randInt() % _container.size()];
    }

    static const char &randSelect(const std::string &_container) {
        if (_container.empty()) {
            xgd::serr << "randSelect empty container" << std::endl;
            exit(-1);
        }
        return _container[randInt() % _container.size()];
    }

    template<typename T>
    static T getSum(const std::vector<T> &_container) {
        T t = 0;
        for (auto &c: _container) {
            t += c;
        }
        return std::move(t);
    }


    template<typename T>
    static const T &randSelect(const std::vector<T> &_container, const std::vector<float> &_probMap) {
        if (_container.empty()) {
            xgd::serr << "randSelect empty container" << std::endl;
            exit(-1);
        }
        if (_container.size() != _probMap.size()) {
            xgd::serr << "randSelect container's size and probMap's size mismatch" << std::endl;
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

    static std::string chopByFirstSpace(const std::string &_str) {
        auto pos = _str.find(" ");
        if (std::string::npos == pos) { return _str; }
        return _str.substr(0, pos);
    }

    template<typename E, typename T>
    static bool notExist(const T &_container, const E &_element) {
        return _container.end() == _container.find(_element);
    }

    static std::string trim(const std::string &str);

    static std::string deleteSubStr(const std::string &_target, const std::string &_subStr);

    static std::string replaceSubStr(const std::string &_target,
                                     const std::string &_subStr, const std::string &_newSubStr);

    static std::vector<std::string_view> split(std::string_view strv, std::string_view delims = " ");
};


struct pair_hash {
    template<class T1, class T2>
    inline std::size_t operator()(const std::pair<T1, T2> &pair) const {
        return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
    }
};

namespace xgd {
    void XGD_BASE_EXPORT printVersion();

    void XGD_BASE_EXPORT recurseDirectory(
            const std::filesystem::path &root,
            const std::function<void(const std::filesystem::path &dir)> &callback
    );

    void XGD_BASE_EXPORT recurseDirectoryWithFilter(
            const std::filesystem::path &root,
            const std::unordered_set<std::string> &ignoredDirectoryName,
            const std::function<void(const std::filesystem::path &dir)> &callback
    );
}