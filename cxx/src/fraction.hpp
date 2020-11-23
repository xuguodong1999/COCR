/**
 * This file is a part of COCR Project
 * @author 徐国栋
 */
#ifndef _FRACTION_HPP_
#define _FRACTION_HPP_

#include <iostream>
#include <sstream>
#include <limits>

template<typename T>
inline T gcd(const T &x, const T &y) {
    return x % y == 0 ? y : gcd(y, x % y);
}

template<typename T>
inline T lcm(const T &x, const T &y) {
    return x * y / gcd(x, y);
}

template<typename T>
class Fraction {

    static_assert(std::is_same_v<T, short> || std::is_same_v<T, int>
                  || std::is_same_v<T, long> || std::is_same_v<T, long long>);

public:
    void set(const T &numerator, const T &denominator = 1) {
        first = numerator;
        second = denominator;
        reduce();
    }

    const T &numerator() const { return first; }

    const T &denominator() const { return second; }

    Fraction() : first(0), second(1) {}

    Fraction(const T &num, const T &den) : first(num), second(den) { reduce(); }

    Fraction(const T &num) : first(num), second(1) {}

    ~Fraction() {}

    Fraction &operator=(const T &integer) {
        first = integer;
        second = 1;
        return *this;
    }

    Fraction &operator=(const Fraction &x) {
        if (this == &x) { return *this; }
        first = x.first;
        second = x.second;
        return *this;
    }

    Fraction operator+(const Fraction &x) const {
        return Fraction(first * x.second + second * x.first, second * x.second);
    }

    Fraction operator-() const {
        return Fraction(-first, second);
    }

    Fraction operator-(const Fraction &x) const {
        return -x + *this;
    }

    Fraction operator*(const Fraction &x) const {
        return Fraction(first * x.first, second * x.second);
    }

    Fraction operator/(const Fraction &x) const {
        return Fraction(first * x.second, second * x.first);
    }

    Fraction &operator+=(const Fraction &x) {
        *this = Fraction(first * x.second + second * x.first, second * x.second);
        return *this;
    }

    Fraction &operator-=(const Fraction &x) {
        return *this += (-x);
    }

    Fraction &operator*=(const Fraction &x) {
        first *= x.first;
        second *= x.second;
        return reduce();
    }

    Fraction &operator/=(const Fraction &x) {
        first *= x.second;
        second *= x.first;
        return reduce();
    }

    bool isZero() const { return first == 0; }

    float floatValue() const {
        return static_cast<float>(first) / static_cast<float>(second);
    }

    bool operator<(const Fraction &x) const {
        return floatValue() - x.floatValue() < 0;
    }

    bool operator<=(const Fraction &x) const {
        return *this == x || *this < x;
    }

    bool operator>(const Fraction &x)const {
        return floatValue() - x.floatValue() > 0;
    }

    bool operator>=(const Fraction &x)const {
        return *this == x || *this > x;
    }

    bool operator==(const Fraction &x) const {
        return first * x.second == second * x.first;
    }

    bool operator!=(const Fraction &x) const {
        return first * x.second != second * x.first;
    }

    Fraction &abs() {
        first = std::abs(first);
        second = std::abs(second);
        return *this;
    }

    void fromString(const std::string &s) {
        std::istringstream is(s);
        is >> *this;
    }

    std::string toString() const {
        std::ostringstream os;
        os << *this;
        return std::move(os.str());
    }

    friend std::ostream &operator<<(std::ostream &out, const Fraction &x) {
        out << "[ " << x.first << " / " << x.second << " ]";
        return out;
    }

    friend std::istream &operator>>(std::istream &in, Fraction &x) {
        char c;
        in >> c >> x.first >> c >> x.second >> c;
        return in;
    }

private:
    T first, second;

    Fraction &reduce() {
        if (!first) {
            second = 1;
        } else if (!second) {
            second = 1;
            first = first > 0 ? std::numeric_limits<T>::max() :
                    std::numeric_limits<T>::lowest();
        } else {
            T x = gcd(first, second);
            first /= x;
            second /= x;
        }
        return *this;
    }
};

using frac = Fraction<int>;

template<typename T>
inline float abs(const Fraction<T> &f) {
    return std::abs(f.floatValue());
}

#endif//_FRACTION_HPP_