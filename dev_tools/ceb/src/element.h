#pragma once

#include "base/fraction.h"
#include <string>

class Element {
public:
    Element();

    Element(std::string &_name);

    ~Element();

    std::string name;
    int amount;
    frac valence;

    void clear();

    bool Exist();

    bool Reduced;
    frac finalValence;
};

bool namelessthan(const Element &a, const Element &b);

