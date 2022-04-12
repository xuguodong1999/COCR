#include "base/hello.h"
using namespace els::base;

Hello::Hello() : c(0) {

}

int Hello::add(const int &a, const int &b) const {
    return a + b + c;
}

int Hello::getC() const {
    return c;
}
