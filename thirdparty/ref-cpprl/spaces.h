#pragma once

#include <string>
#include <vector>

namespace cpprl {
    struct ActionSpace {
        std::string type;
        std::vector<int64_t> shape;
    };
}