#include "expand_util.hpp"

using namespace xgd;

std::unordered_map<std::string, AbbType> xgd::SUPER_ATOM_MAP = {};

/**
 * 我在毕设里要达成的目标：基于字符串解析的残基拆解，顺序解析
 * 1、支持单层括号，支持下标数字，支持化学键符号，支持单电荷
 * 2、不支持谓词语义（包括左计数和异构符），不支持多电荷
 * TODO:
 * 1、电荷解析通过词法分析做
 * 2、没有分隔符，请使用最长单词试探法
 */
bool xgd::tryExpand(JMol &mol, std::shared_ptr<JAtom> atom) {
    return true;
}

void xgd::initSuperAtomMap() {
    if (!SUPER_ATOM_MAP.empty()) { return; }
    // 元素语义
    for (auto&[str, data]:STR_ELEMENT_MAP) {
        SUPER_ATOM_MAP[str] = AbbType::Element;
    }
    // 数字语义
    for (int i = 0; i <= 9; i++) {
        SUPER_ATOM_MAP[std::to_string(i)] = AbbType::Number;
    }
    // 电荷语义
    SUPER_ATOM_MAP["+"] = AbbType::Pos;
    SUPER_ATOM_MAP["_"] = AbbType::Neg;
    // 化学键语义
    SUPER_ATOM_MAP["-"] = AbbType::Single;
    SUPER_ATOM_MAP["="] = AbbType::Double;
    SUPER_ATOM_MAP["#"] = AbbType::Triple;
    // 括号语义
    SUPER_ATOM_MAP["("] = AbbType::Left;
    SUPER_ATOM_MAP[")"] = AbbType::Right;
    // 缩写语义
    SUPER_ATOM_MAP["Me"] = AbbType::Me;
    SUPER_ATOM_MAP["Et"] = AbbType::Et;
    SUPER_ATOM_MAP["Ethyl"] = AbbType::Et;
    SUPER_ATOM_MAP["iBu"];
    SUPER_ATOM_MAP["tBu"];
    SUPER_ATOM_MAP["nBu"];
    SUPER_ATOM_MAP["Bu"];
    SUPER_ATOM_MAP["NO2"];
    SUPER_ATOM_MAP["NO"];
    SUPER_ATOM_MAP["SO3"];
    SUPER_ATOM_MAP["Ac"];
    SUPER_ATOM_MAP["COO"];
    SUPER_ATOM_MAP["CO2"];
    SUPER_ATOM_MAP["SOO"];
    SUPER_ATOM_MAP["SO2"];
    SUPER_ATOM_MAP["Ph"];
    SUPER_ATOM_MAP["Bz"];
    SUPER_ATOM_MAP["THPO"];
    SUPER_ATOM_MAP["NHZ"];
    SUPER_ATOM_MAP["Bn"];
    SUPER_ATOM_MAP["Am"];
    SUPER_ATOM_MAP["Am"];
    SUPER_ATOM_MAP["Cbz"];
    SUPER_ATOM_MAP["Ms"];
    SUPER_ATOM_MAP["OCN"];
    SUPER_ATOM_MAP["NCO"];
    SUPER_ATOM_MAP["NCS"];
    SUPER_ATOM_MAP["SCN"];
    SUPER_ATOM_MAP["D"];
    SUPER_ATOM_MAP["Ace"];
    SUPER_ATOM_MAP["THP"];
    // TODO:
}
