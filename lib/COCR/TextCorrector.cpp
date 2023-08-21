#include "XGD/COCR/TextCorrector.h"
#include "XGD/Base/Util.h"
#include "XGD/Base/ElementType.h"

#include <vector>
#include <algorithm>


std::string TextCorrector::correct(const std::string &_text) {
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
    result = StdUtil::replaceSubStr(result, "OD", "OO");
    result = StdUtil::replaceSubStr(result, "DD", "OO");
    result = StdUtil::replaceSubStr(result, "DO", "OO");
    result = StdUtil::replaceSubStr(result, "SH", "OO");
    result = StdUtil::replaceSubStr(result, "HD", "HO");
    result = StdUtil::replaceSubStr(result, "DH", "OH");
    result = StdUtil::replaceSubStr(result, "CL", "Cl");
    result = StdUtil::replaceSubStr(result, "CZ", "C2");
    result = StdUtil::replaceSubStr(result, "(Z", "C2");
    result = StdUtil::replaceSubStr(result, "CbZ", "Cbz");
    result = StdUtil::replaceSubStr(result, "CeO", "COO");
    result = StdUtil::replaceSubStr(result, "eOO", "_OO");
    int lb = 0, rb = 0;
    for (auto &c: result) {
        if (c == '(') { ++lb; } else if (c == ')') { ++rb; }
    }
    if (lb < rb) {
        int delta = rb - lb;
        for (auto &c: result) {
            if (c == 'C') {
                c = '(';
                if (!--delta) { break; }
            }
        }
    } else if (lb > rb) {
        int delta = lb - rb;
        for (auto &c: result) {
            if (c == '(') {
                c = 'C';
                if (!--delta) { break; }
            }
        }
    }
    return result;
}

const std::string &TextCorrector::correct2(const std::string &_text) {
    auto it = c2Map.find(_text);
    if (c2Map.end() == it) {
        return _text;
    } else {
        return it->second;
    }
}

std::vector<std::string> TextCorrector::similarChar = {
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

void TextCorrector::InitData() {
    for (auto &sim: similarChar) {
        for (size_t i = 0; i < sim.length(); i++) {
            for (size_t j = i + 1; j < sim.length(); j++) {
                char a = sim[i], b = sim[j];
                ccMap.insert({a, b});
                ccMap.insert({b, a});
            }
        }
    }
    for (size_t i = 0; i < ElementUtil::GetElements().size(); i++) {
        if (i > 53)break;// FIXME: Ho -> HO ?
        const auto &ele = ElementUtil::GetElements()[i];
        std::string upperCase;
        upperCase.resize(ele.length());
        std::transform(ele.begin(), ele.end(), upperCase.begin(), [](char c) { return std::toupper(c); });
        for (size_t j = 0; j < upperCase.length(); j++) {
            std::string tmpStr = upperCase;
            auto [beg, end] = ccMap.equal_range(upperCase[j]);
            while (beg != end) {
                tmpStr[j] = beg->second;
                c2Map[tmpStr] = ele;
//                xgd::sout << tmpStr << "," << ele << std::endl;
                ++beg;
            }
        }
        c2Map[std::move(upperCase)] = ele;
    }
}

const std::string &TextCorrector::GetAlphabet() {
    return ALPHABET;
}
