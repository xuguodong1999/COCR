#include "math/hello.h"
using namespace els::math;

Hello::Hello() : c(0) {

}

int Hello::add(const int &a, const int &b) const {
    return a + b + c;
}

int Hello::getC() const {
    return c;
}
