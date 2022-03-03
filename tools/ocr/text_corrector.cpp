#include "text_corrector.hpp"

#include <vector>
#include <algorithm>

namespace cocr {
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


std::string cocr::TextCorrector::correct(const std::string &_text) {
    std::string result = _text;
    for (size_t i = 0; i < result.size(); i++) {
        auto &c = result[i];
        if (i > 0) {
            auto c0 = result[i - 1];
            if (c0 == '(' && '0' <= c && c <= '9') {
                c = 'C';// (x -> Cx
            }
        }
//        if (i < result.size() - 1) {
//            auto c1 = result[i + 1];
//        }
    }
    switch (result.length()) {
        case 1:
            if (result == "D" || result == "0") {
                result = "O";
            }
            break;
        case 2:
            return correct2(result);
        case 3:
            break;
        case 4:
            break;
    }
    result = cocr::replaceSubStr(result, "OD", "OO");
    result = cocr::replaceSubStr(result, "DD", "OO");
    result = cocr::replaceSubStr(result, "DO", "OO");
    result = cocr::replaceSubStr(result, "SH", "OO");
    result = cocr::replaceSubStr(result, "HD", "HO");
    result = cocr::replaceSubStr(result, "DH", "OH");
    result = cocr::replaceSubStr(result, "CL", "Cl");
    result = cocr::replaceSubStr(result, "CZ", "C2");
    result = cocr::replaceSubStr(result, "(Z", "C2");
    result = cocr::replaceSubStr(result, "CbZ", "Cbz");
    result = cocr::replaceSubStr(result, "CeO", "COO");
    result = cocr::replaceSubStr(result, "eOO", "_OO");
    int lb = 0, rb = 0;
    for (auto &c:result) {
        if (c == '(') { ++lb; } else if (c == ')') { ++rb; }
    }
    if (lb < rb) {
        int delta = rb - lb;
        for (auto &c:result) {
            if (c == 'C') {
                c = '(';
                if (!--delta) { break; }
            }
        }
    } else if (lb > rb) {
        int delta = lb - rb;
        for (auto &c:result) {
            if (c == '(') {
                c = 'C';
                if (!--delta) { break; }
            }
        }
    }
    return result;
}

const std::string &cocr::TextCorrector::correct2(const std::string &_text) {
    auto it = c2Map.find(_text);
    if (c2Map.end() == it) {
        return _text;
    } else {
        return it->second;
    }
}

std::vector<std::string> cocr::TextCorrector::similarChar = {
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

void cocr::TextCorrector::InitData() {
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

const std::string &cocr::TextCorrector::GetAlphabet() {
    return ALPHABET;
}
