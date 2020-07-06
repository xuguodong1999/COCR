#ifndef _FRACTION_H_
#define _FRACTION_H_

#include <string>
#include <iostream>
#include <sstream>
#include <cassert>
#include <limits>

namespace cocr {
    using namespace std;

    class Fraction {
    private:
        int first, second;

        int gcd(const int &x, const int &y) const {
            return x % y == 0 ? y : gcd(y, x % y);
        }

        Fraction &reduce() {
            if (!first) {
                second = 1;
            } else if (!second) {
                second = 1;
                first = first > 0 ? numeric_limits<int>::max() : numeric_limits<int>::min();
            } else {
                int x = gcd(first, second);
                first /= x;
                second /= x;
            }
            return *this;
        }

    public:
        void set(const int &numerator, const int &denominator = 1) {
            first = numerator;
            second = denominator;
            assert(denominator != 0);
            reduce();
        }

        const int &numerator() const { return first; }

        const int &denominator() const { return second; }

        Fraction() : first(0), second(1) {}

        Fraction(const int &num, const int &den) : first(num), second(den) { reduce(); }

        Fraction(const int &num) : first(num), second(1) {}

        ~Fraction() {}

        Fraction &operator=(const int &integer) {
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
            return *this != x;
        }

        Fraction &abs() {
            first = std::abs(first);
            second = std::abs(second);
            return *this;
        }

        void fromString(const string &s) {
            istringstream is(s);
            is >> *this;
        }

        string toString() const {
            ostringstream os;
            os << *this;
            return os.str();
        }

        friend ostream &operator<<(ostream &out, const Fraction &x) {
            out << x.first << " / " << x.second;
            return out;
        }

        friend istream &operator>>(istream &in, Fraction &x) {
            char c;
            in >> x.first >> c >> x.second;
            return in;
        }
    };
}
#endif //!_FRACTION_H_
