#include "hw_data.hpp"
#include <QFile>
#include <sstream>
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>

static std::vector<int> sLabelCharLikeLine = {
        8, 24, 53,
        130, 134, 141, 146, 158,
        172
};
static std::vector<int> sLabelCharLikeCircle = {9, 21, 47};
static std::map<std::string, int> sStr2IntMap = {
        {"!",  183},
        {"Ⅴ",  182},
        {"\"", 181},
        {"Ⅳ",  180},
        {"θ",  179},
        {"#",  178},
        {"Ⅲ",  177},
        {"￥",  176},
        {"%",  173},
        {"ξ",  174},
        {"Ⅱ",  175},
        {"&",  171},
        {"Ⅰ",  172},
        {"`",  169},
        {"μ",  170},
        {"(",  167},
        {"γ",  168},
        {")",  165},
        {"β",  166},
        {"*",  163},
        {"α",  164},
        {"+",  162},
        {",",  160},
        {"η",  161},
        {"-",  158},
        {"ζ",  159},
        {".",  156},
        {"ε",  157},
        {"/",  154},
        {"δ",  155},
        {":",  152},
        {"ρ",  153},
        {";",  149},
        {"{",  150},
        {"π",  151},
        {"<",  145},
        {"|",  146},
        {"÷",  147},
        {"ψ",  148},
        {"=",  143},
        {"}",  144},
        {">",  140},
        {"￣",  141},
        {"φ",  142},
        {"?",  139},
        {"@",  138},
        {"[",  136},
        {"Π",  137},
        {"\\", 134},
        {"Ψ",  135},
        {"]",  133},
        {"^",  131},
        {"Φ",  132},
        {"_",  130},
        {"｀",  128},
        {"λ",  129},
        {"《",  127},
        {"》",  126},
        {"$",  124},
        {"Σ",  125},
        {"Ω",  123},
        {"σ",  122},
        {"τ",  121},
        {"ω",  120},
        {"Ⅵ",  119},
        {"Ⅶ",  118},
        {"Ⅷ",  117},
        {"Ⅸ",  116},
        {"Ⅹ",  115},
        {"Ⅺ",  114},
        {"Ⅻ",  113},
        {"ⅰ",  112},
        {"ⅱ",  111},
        {"ⅲ",  110},
        {"ⅳ",  109},
        {"ⅴ",  108},
        {"ⅵ",  107},
        {"ⅶ",  106},
        {"ⅷ",  105},
        {"ⅸ",  104},
        {"ⅹ",  103},
        {"←",  102},
        {"↑",  101},
        {"→",  100},
        {"↓",  99},
        {"↖",  98},
        {"↗",  97},
        {"↘",  96},
        {"↙",  95},
        {"∈",  94},
        {"√",  93},
        {"∞",  92},
        {"∫",  91},
        {"∮",  90},
        {"∴",  89},
        {"∵",  88},
        {"≈",  87},
        {"≌",  86},
        {"≠",  85},
        {"≤",  84},
        {"≥",  83},
        {"⊕",  81},
        {"7",  82},
        {"⊙",  80},
        {"±",  79},
        {"①",  78},
        {"②",  77},
        {"③",  76},
        {"④",  75},
        {"⑤",  74},
        {"⑥",  73},
        {"⑦",  72},
        {"⑧",  71},
        {"⑨",  70},
        {"⑩",  69},
        {"￡",  68},
        {"、",  67},
        {"‘",  66},
        {"’",  65},
        {"“",  64},
        {"”",  63},
        {"。",  62},
        {"A",  61},
        {"B",  60},
        {"C",  59},
        {"D",  58},
        {"E",  57},
        {"F",  56},
        {"G",  55},
        {"H",  54},
        {"I",  53},
        {"J",  52},
        {"K",  51},
        {"L",  50},
        {"M",  49},
        {"N",  48},
        {"O",  47},
        {"P",  46},
        {"Q",  45},
        {"R",  44},
        {"S",  43},
        {"T",  42},
        {"U",  41},
        {"V",  40},
        {"W",  39},
        {"X",  38},
        {"Y",  37},
        {"Z",  36},
        {"a",  35},
        {"b",  34},
        {"c",  33},
        {"d",  32},
        {"e",  31},
        {"f",  30},
        {"g",  29},
        {"h",  28},
        {"i",  27},
        {"j",  26},
        {"k",  25},
        {"l",  24},
        {"m",  23},
        {"n",  22},
        {"o",  21},
        {"p",  20},
        {"q",  19},
        {"r",  18},
        {"s",  17},
        {"t",  16},
        {"u",  15},
        {"v",  14},
        {"w",  13},
        {"x",  12},
        {"y",  11},
        {"z",  10},
        {"0",  9},
        {"1",  8},
        {"2",  7},
        {"3",  6},
        {"4",  5},
        {"5",  4},
        {"6",  3},
        {"8",  2},
        {"9",  1}
};

HwScript HwDataLoader::GetByIntLabel(int label) {
    if (label >= mData.size()) {
        std::cerr << "label " << label << " out of boundary: [0," <<
                  mData.size() << ")" << std::endl;
        exit(-1);
    }
    auto &classData = mData[label];
    size_t idx = rand() % classData.size();
    if (label == 43) {
        // 补丁：这个字符有问题
        while (idx == 43) idx = rand() % classData.size();
    }
    return classData[idx].toHwScript();
}

HwScript HwDataLoader::GetByStrLabel(const std::string &label) {
    auto resItr = sStr2IntMap.find(label);
    if (resItr == sStr2IntMap.end()) {
        std::cerr << "fail to find " << label
                  << " in HWChar::sStr2IntMap" << std::endl;
        exit(-1);
    }
    return std::move(GetByIntLabel(resItr->second));
}

HwScript HwDataLoader::GetShape(const ShapeType&_shapeType) {
    switch(_shapeType){
        case ShapeType::Line:{
            return std::move(GetByIntLabel(
                    sLabelCharLikeLine[rand() % sLabelCharLikeLine.size()]));
        }case ShapeType::Circle:{
            auto circle = GetByIntLabel(
                    sLabelCharLikeCircle[rand() % sLabelCharLikeCircle.size()]);
            circle.castToCircle(cv::Point2f(50, 50), 50, 50);
            return std::move(circle);
        }
        default:{
            std::cerr << (int)_shapeType<< " not supported! "
                      << "Only \"line\" and \"circle\" supported!" << std::endl;
            exit(-1);
        }
    }
}

void HwDataLoader::LoadCouchDataSet() {
    mData.clear();
    std::shared_ptr<std::istream> ism;
    bool fromQrc = (COUCH_SYM_PATH.length() > 2 && COUCH_SYM_PATH[0] == ':');
    if (fromQrc) {
        QFile loader(COUCH_SYM_PATH.c_str());
        if (!loader.open(QIODevice::ReadOnly)) {
            std::cerr << "fail to load " << COUCH_SYM_PATH << " with qrc" << std::endl;
            exit(-1);
        }
        ism = std::make_shared<std::stringstream>(loader.readAll().toStdString());
        loader.close();
    } else {
        auto fsm = std::make_shared<std::ifstream>(
                COUCH_SYM_PATH, std::ios::in | std::ios::binary);
        ism = fsm;
        if (!fsm->is_open()) {
            std::cerr << "fail to load " << COUCH_SYM_PATH << " with ifstream" << std::endl;
            exit(-1);
        }
    }
    int a, b, c, d, e, iii = 0;
    // 【标签索引】、【宽度】、【长度】、【笔画数目】
    while (ism->read(reinterpret_cast<char *>(&a), 4)) {
        if (++iii % 10000 == 0) {
            std::cout << iii << std::endl;
        }
        ism->read(reinterpret_cast<char *>(&b), 4);
        ism->read(reinterpret_cast<char *>(&c), 4);
        ism->read(reinterpret_cast<char *>(&d), 4);
        std::vector<std::vector<cv::Point2f>> script;
        script.resize(d);
        for (int i = 0; i < d; i++) {
            // 【点数】
            ism->read(reinterpret_cast<char *>(&e), 4);
            script[i].resize(e);
            for (int j = 0; j < e; j++) {
                ism->read(reinterpret_cast<char *>(&script[i][j].x), 4);
                ism->read(reinterpret_cast<char *>(&script[i][j].y), 4);
            }
        }
        if (a >= mData.size()) {
            mData.resize(a + 1);
        }
        mData[a].push_back(std::move(script));
    }
    if (!fromQrc) {
        std::static_pointer_cast<std::ifstream>(ism)->close();
    }
    for (auto &dat:mData) {
        dat.resize(dat.size());
    }
}

HwScript HwDataLoader::GetHwScript(const size_t &_classIndex, const size_t &_sampleIndex) {
    auto &sample = mData[_classIndex][_sampleIndex];
    return sample.toHwScript();
}

HwDataLoader::HwDataLoader() : isDataLoaded(false) {
}

HwDataLoader &HwDataLoader::getInstance() {
    static HwDataLoader e;
    if (!e.isDataLoaded) {
        e.LoadCouchDataSet();
        e.isDataLoaded=true;
    }
    return e;
}

HwDataSample::HwDataSample(vector <std::vector<cv::Point2f>> &&_data) {
    swap(_data);
}

HwScript HwDataSample::toHwScript() const {
    HwScript hwScript;
    for (auto &stroke:*this) {
        if (stroke.empty())continue;
        HwStroke hwStroke;
        for (auto &pt:stroke) {
            hwStroke.push_back(pt);
        }
        hwScript.push_back(hwStroke);
    }
    return std::move(hwScript);
}
