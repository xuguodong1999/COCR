#include "ckit/hello.h"

using namespace els::ckit;

Hello::Hello() : hello() {

}

int Hello::add(const int &a, const int &b) const {
    return hello.add(a, b);
}

int Hello::getC() const {
    return hello.getC();
}
