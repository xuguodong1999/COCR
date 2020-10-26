/**
 * 版权声明：本文为博主原创文章，遵循 CC 4.0 BY-SA 版权协议，转载请附上原文出处链接和本声明。
 * 本文链接：https://blog.csdn.net/shutdownYourComputer/article/details/88865909
 */
#include "bignumber.hpp"

std::ostream &operator<<(std::ostream &os, const UnsignedInteger &num) {
    for (int i = num.bufferSize - 1; i >= 0; i--) {
        os << (int) num.buffer[i];
    }
    return os;
}

UnsignedInteger::UnsignedInteger() {
    memset(buffer, 0, sizeof(buffer));
    bufferSize = 0;
}

UnsignedInteger::UnsignedInteger(long long num) {
    *this = num;
}

UnsignedInteger::UnsignedInteger(const std::string &str) {
    *this = str;
}

UnsignedInteger UnsignedInteger::operator=(long long num) {
    memset(buffer, 0, sizeof(buffer));
    bufferSize = 0;
    do {
        buffer[bufferSize++] = num % 10;
        num /= 10;
    } while (num);
    return *this;
}

UnsignedInteger UnsignedInteger::operator=(const std::string &str) {
    memset(buffer, 0, sizeof(buffer));
    bufferSize = str.size();
    for (int i = 0; i < bufferSize; i++) {
        buffer[i] = str[bufferSize - i - 1] - '0';
    }
    return *this;
}

bool UnsignedInteger::operator<(const UnsignedInteger &b) const {
    if (bufferSize != b.bufferSize)return bufferSize < b.bufferSize;
    for (int i = bufferSize - 1; i >= 0; i--) {
        if (buffer[i] != b.buffer[i])return buffer[i] < b.buffer[i];
    }
    return false;
}

UnsignedInteger UnsignedInteger::operator+(const UnsignedInteger &b) const {
    UnsignedInteger c;
    int carry = 0;
    for (int i = 0;; i++) {
        if (carry == 0 && i >= bufferSize && i >= b.bufferSize)break;
        int x = carry;
        if (i < bufferSize)x += buffer[i];
        if (i < b.bufferSize)x += b.buffer[i];
        c.buffer[c.bufferSize++] = x % 10;
        carry = x / 10;
    }
    return c;
}

UnsignedInteger UnsignedInteger::operator+=(const UnsignedInteger &b) {
    *this = *this + b;
    return *this;
}

UnsignedInteger UnsignedInteger::operator-(const UnsignedInteger &b) const {// *this >= b
    UnsignedInteger c;
    bool borrow = false;
    for (int i = 0; i < bufferSize; i++) {
        int x = borrow ? buffer[i] - 1 : buffer[i];
        if (x < b.buffer[i]) {
            borrow = true;
            x += 10;
        } else {
            borrow = false;
        }
        c.buffer[c.bufferSize++] += x - b.buffer[i];
    }
    while (c.buffer[c.bufferSize - 1] == 0 && c.bufferSize > 1)c.bufferSize--;
    return c;
}

UnsignedInteger UnsignedInteger::operator-=(const UnsignedInteger &b) {
    *this = *this - b;
    return *this;
}

UnsignedInteger UnsignedInteger::operator*(const UnsignedInteger &b) const {
    UnsignedInteger c(0);
    for (int i = 0; i < bufferSize; i++) {
        UnsignedInteger t;
        t.bufferSize = i;
        int carry = 0;
        for (int j = 0; j < b.bufferSize; j++) {
            int x = buffer[i] * b.buffer[j] + carry;
            t.buffer[t.bufferSize++] = x % 10;
            carry = x / 10;
        }
        if (carry) {
            t.buffer[t.bufferSize++] = carry;
        }
        c += t;
    }
    UnsignedInteger e;
    e.bufferSize = c.bufferSize;
    if (c == e) {
        return UnsignedInteger(0);
    }
    return c;
}

UnsignedInteger UnsignedInteger::operator*=(const UnsignedInteger &b) {
    *this = *this * b;
    return *this;
}

UnsignedInteger UnsignedInteger::operator^(long long num) const {
    UnsignedInteger b(*this);
    UnsignedInteger c(1);
    while (num) {
        if ((num & 1) == 1) {
            c *= b;
        }
        b *= b;
        num >>= 1;
    }
    return c;
}

UnsignedInteger UnsignedInteger::operator/(const UnsignedInteger &b) const {
    if (*this < b) {
        return UnsignedInteger(0);
    }
    UnsignedInteger c;
    UnsignedInteger d(10);
    UnsignedInteger m(*this);
    UnsignedInteger n;
    int digit = bufferSize - b.bufferSize;
    for (int i = digit; i >= 0; i--) {
        n = b * (d ^ i);
        int j = 0;
        for (;; j++) {
            if (m >= n) {
                m = m - n;
            } else {
                break;
            }
        }
        UnsignedInteger t(j);
        c += t * (d ^ i);
    }
    return c;
}

UnsignedInteger UnsignedInteger::operator/=(const UnsignedInteger &b) {
    *this = *this / b;
    return *this;
}