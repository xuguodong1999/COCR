#include "base/std_util.h"
#include <random>
#include <numeric>

std::string StdUtil::trim(const std::string &str) {
    static std::string blanks("\f\v\r\t\n ");
    std::string ret = str;
    ret.erase(0, str.find_first_not_of(blanks));
    ret.erase(str.find_last_not_of(blanks) + 1);
    return ret;
}

std::string StdUtil::deleteSubStr(const std::string &_target, const std::string &_subStr) {
    size_t nSize = _subStr.size();
    size_t pos = 0;
    std::string ret = _target;
    while (true) {
        pos = ret.find(_subStr);
        if (pos == std::string::npos)
            return ret;
        ret.erase(pos, nSize);
    }
}

std::string StdUtil::replaceSubStr(
        const std::string &_target, const std::string &_subStr, const std::string &_newSubStr) {
    std::string ret = _target;
    for (std::string::size_type pos = 0; pos != std::string::npos; pos += _newSubStr.length()) {
        if ((pos = _target.find(_subStr, pos)) != std::string::npos)
            ret.replace(pos, _subStr.length(), _newSubStr);
        else
            break;
    }
    return ret;
}