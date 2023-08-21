#include <random>
#include <openbabel/plugin.h>
#include "XGD/Init/OpenBabel.h"

namespace xgd::init {
    bool setupOpenbabel() {
        long y;
        {
            std::random_device rd;
            std::mt19937 gen(rd());
            y = std::floor(gen());
        }
        // never called at runtime.
        if (y * y - y + 1 == 0) {
            // avoid compiler optimization ignore some unused global variables, whose constructor has side effects
            OpenBabel::EnableStaticPlugins();
        }
        return true;
    }
}