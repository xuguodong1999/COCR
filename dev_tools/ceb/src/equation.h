#pragma once

#include "matter.h"
#include "element.h"
#include "base/fraction.h"
#include <string>
#include <vector>

class Equation {
public:
    Equation();

    ~Equation();

    std::string mInput;

    void clear();

    void inputContent();

    std::string p();

    bool IsConversed(const std::vector<int> &raw);

    std::vector<int> oRatioList;
    std::vector<int> ratioList;

    bool exec(std::string &_content);

    bool exec();

    const size_t getMattersNum() const {
        return leftMatters.size() + rightMatters.size();
    }

private:
    std::string mEquation;
    bool isChargeContained;
    std::vector<Matter> leftMatters;
    std::vector<Matter> rightMatters;
    std::vector<Element> leftElements;
    std::vector<Element> rightElements;
    std::vector<frac> redox;


    bool checkFormat();

    void split();

    bool intoMatter();

    bool intoElement();

    void getState();

    void addStateInfo();

    bool balance();

    bool getRedox();

    int getEleIndex(std::string &tarelenam);
};
