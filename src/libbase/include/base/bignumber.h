/**
 * 版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
 * 本文链接：https://blog.csdn.net/shutdownYourComputer/article/details/88865909
 */
#pragma once

#include "els_base_export.h"

#include <cstdio>
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

class ELS_BASE_EXPORT UnsignedInteger {
    unsigned char buffer[256];
    size_t bufferSize;
public:
    UnsignedInteger();

    UnsignedInteger(long long num);

    UnsignedInteger(const std::string &str);

    UnsignedInteger operator=(long long num);

    UnsignedInteger operator=(const std::string &str);

    bool operator<(const UnsignedInteger &b) const;

    bool operator>(const UnsignedInteger &b) const { return b < *this; }

    bool operator<=(const UnsignedInteger &b) const { return !(b < *this); }

    bool operator>=(const UnsignedInteger &b) const { return !(*this < b); }

    bool operator!=(const UnsignedInteger &b) const { return b < *this || *this < b; }

    bool operator==(const UnsignedInteger &b) const { return !(b < *this) && !(*this < b); }

    UnsignedInteger operator+(const UnsignedInteger &b) const;

    UnsignedInteger operator+=(const UnsignedInteger &b);

    UnsignedInteger operator-(const UnsignedInteger &b) const;

    UnsignedInteger operator-=(const UnsignedInteger &b);

    UnsignedInteger operator*(const UnsignedInteger &b) const;

    UnsignedInteger operator*=(const UnsignedInteger &b);

    UnsignedInteger operator^(long long num) const;

    UnsignedInteger operator/(const UnsignedInteger &b) const;

    UnsignedInteger operator/=(const UnsignedInteger &b);

    friend ELS_BASE_EXPORT std::ostream &operator<<(std::ostream &os, const UnsignedInteger &num);
};