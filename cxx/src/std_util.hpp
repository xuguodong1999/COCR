/**
 * This file is a part of COCR Project
 * @author 徐国栋
 */
#ifndef _STD_UTIL_HPP_
#define _STD_UTIL_HPP_

#include <string>


std::string trim(const std::string &str);

std::string deleteSubStr(const std::string &_target, const std::string &_subStr);

std::string replaceSubStr(const std::string &_target,
                          const std::string &_subStr, const std::string &_newSubStr);

#endif//_STD_UTIL_HPP_
