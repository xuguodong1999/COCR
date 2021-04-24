#include "text_corrector.hpp"

#include <vector>
#include <algorithm>

namespace xgd {
    extern std::vector<std::string> ELEMENT_NAME_LIST;

    std::string replaceSubStr(const std::string &_target,
                              const std::string &_subStr, const std::string &_newSubStr) {
        std::string ret = _target;
        for (std::string::size_type pos = 0; pos != std::string::npos; pos += _newSubStr.length()) {
            if ((pos = _target.find(_subStr, pos)) != std::string::npos)
                ret.replace(pos, _subStr.length(), _newSubStr);
            else
                break;
        }
        return move(ret);
    }
}


std::string xgd::TextCorrector::correct(const std::string &_text) {
    std::string result = _text;
    switch (result.length()) {
        case 1:
            break;
        case 2:
            return correct2(result);
        case 3:
            break;
        case 4:
            break;
    }
    result = xgd::replaceSubStr(result, "OD", "OO");
    result = xgd::replaceSubStr(result, "DD", "OO");
    result = xgd::replaceSubStr(result, "DO", "OO");
    result = xgd::replaceSubStr(result, "SH", "OO");
    result = xgd::replaceSubStr(result, "HD", "HO");
    result = xgd::replaceSubStr(result, "DH", "OH");
    return result;
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
