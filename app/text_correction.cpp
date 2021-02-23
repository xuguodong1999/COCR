#include "text_correction.hpp"
#include <vector>
#include <algorithm>
#include <iostream>

namespace xgd {
    extern std::vector<std::string> ELEMENT_NAME_LIST;
}

std::vector<std::string> xgd::TextCorrector::correct(const std::string &_text) {
    return std::vector<std::string>();
}

const std::string &xgd::TextCorrector::correct2(const std::string &_text) {
    auto it = c2Map.find(_text);
    if (c2Map.end() == it) {
        return _text;
    } else {
        return it->second;
    }
}

std::vector<std::string> xgd::TextCorrector::similarChar = {
        "A4",
        "DO0",
        "B83",
        "CG",
        "IL1",
        "S5",
        "UV",
        "Z2",
        "b6",
        "fg",
        "ij",
        "q9",
};

void xgd::TextCorrector::InitData() {
    for (auto &sim:similarChar) {
        for (size_t i = 0; i < sim.length(); i++) {
            for (size_t j = i + 1; j < sim.length(); j++) {
                char a = sim[i], b = sim[j];
                ccMap.insert({a, b});
                ccMap.insert({b, a});
            }
        }
    }
    for (size_t i = 0; i < ELEMENT_NAME_LIST.size(); i++) {
        if (i > 53)break;// FIXME: Ho -> HO ?
        const auto &ele = ELEMENT_NAME_LIST[i];
        std::string upperCase;
        upperCase.resize(ele.length());
        std::transform(ele.begin(), ele.end(), upperCase.begin(), [](char c) { return std::toupper(c); });
        for (size_t j = 0; j < upperCase.length(); j++) {
            std::string tmpStr = upperCase;
            auto[beg, end]=ccMap.equal_range(upperCase[j]);
            while (beg != end) {
                tmpStr[j] = beg->second;
                c2Map[tmpStr] = ele;
//                std::cout << tmpStr << "," << ele << std::endl;
                ++beg;
            }
        }
        c2Map[std::move(upperCase)] = ele;
    }
}

const std::string &xgd::TextCorrector::GetAlphabet() {
    return ALPHABET;
}
