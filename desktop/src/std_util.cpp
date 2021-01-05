/**
 * This file is a part of COCR Project
 * @author 徐国栋
 */
#include "std_util.hpp"

using namespace std;

string trim(const string &str) {
    static string blanks("\f\v\r\t\n ");
    string ret = str;
    ret.erase(0, str.find_first_not_of(blanks));
    ret.erase(str.find_last_not_of(blanks) + 1);
    return move(ret);
}

string deleteSubStr(const string &_target, const string &_subStr) {
    size_t nSize = _subStr.size();
    size_t pos = 0;
    string ret = _target;
    while (true) {
        pos = _target.find(_subStr);
        if (pos == string::npos)
            return move(ret);
        ret.erase(pos, nSize);
    }
}

string replaceSubStr(const string &_target,
                     const string &_subStr, const string &_newSubStr) {
    string ret = _target;
    for (string::size_type pos = 0; pos != string::npos; pos += _newSubStr.length()) {
        if ((pos = _target.find(_subStr, pos)) != string::npos)
            ret.replace(pos, _subStr.length(), _newSubStr);
        else
            break;
    }
    return move(ret);
}