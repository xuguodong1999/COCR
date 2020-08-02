#ifndef _FRACTION_H_
#define _FRACTION_H_

#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <limits>
#include <algorithm>

namespace cocr {
    inline const float &eps() {
        return 0.0000000000000001;
    }

    inline const float &epsNeg() {
        return -0.0000000000000001;
    }

    template<typename T>
    class Fraction {
    public:
        void set(const T &numerator, const T &denominator = 1) {
            first = numerator;
            second = denominator;
            if (denominator == 0) {
                denominator = cocr::eps();
            }
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

        float floatValue() const {
            return first / second;
        }

        bool operator<(const Fraction &x) const {
            return floatValue() - x.floatValue() < 0;
        }

        bool operator<=(const Fraction &x) const {
            return *this == x || *this < x;
        }

        bool operator>(const Fraction &x) {
            return floatValue() - x.floatValue() > 0;
        }

        bool operator>=(const Fraction &x) {
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
            return os.str();
        }

        friend std::ostream &operator<<(std::ostream &out, const Fraction &x) {
            out << x.first << " / " << x.second;
            return out;
        }

        friend std::istream &operator>>(std::istream &in, Fraction &x) {
            char c;
            in >> x.first >> c >> x.second;
            return in;
        }

    private:
        T first, second;

        T gcd(const T &x, const T &y) const {
            return x % y == 0 ? y : gcd(y, x % y);
        }

        Fraction &reduce() {
            if (!first) {
                second = 1;
            } else if (!second) {
                second = 1;
                first = first > 0 ? std::numeric_limits<T>::max() : std::numeric_limits<T>::min();
            } else {
                T x = gcd(first, second);
                first /= x;
                second /= x;
            }
            return *this;
        }
    };

    typedef Fraction<long long> frac;

    template<typename T>
    inline float abs(const Fraction<T> &f) {
        return std::abs(f.floatValue());
    }
}
#endif //!_FRACTION_H_
