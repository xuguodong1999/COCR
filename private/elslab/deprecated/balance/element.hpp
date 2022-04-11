#ifndef _BALANCE_ELEMENT_HPP_
#define _BALANCE_ELEMENT_HPP_

#include "fraction.hpp"
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

#endif//_BALANCE_ELEMENT_HPP_
