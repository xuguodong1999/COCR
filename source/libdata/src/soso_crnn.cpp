#include "data/soso_crnn.h"

#include "base/std_util.h"
#include "stroke/str.h"
#include "data/g_chem_text.h"
#include "ocv/algorithm.h"

#include <QPainter>
#include <QTextDocument>
#include <QFontDatabase>
#include <QDebug>

#include <filesystem>
#include <fstream>
#include <iostream>

static std::vector<std::string> availableFontFamilies;

CRNNDataGenerator::CRNNDataGenerator() : isInited(false) {
    alphabetSet.clear();
    for (auto &c: SOSO_ALPHABET) {
        alphabetSet.insert(c);
    }
}

bool CRNNDataGenerator::init(const std::string &_topDir) {
    topDir = _topDir;
    if (!std::filesystem::exists(topDir)) {
        if (!std::filesystem::create_directories(topDir)) {
            std::cerr << "fail to create dir: " << topDir << std::endl;
            exit(-1);
        }
    }
    auto imgPath = topDir + trainDir + imgDir;
    if (!std::filesystem::exists(imgPath)) {
        if (!std::filesystem::create_directories(imgPath)) {
            std::cerr << "fail to create dir: " << imgPath << std::endl;
            exit(-1);
        }
    }
    imgPath = topDir + testDir + imgDir;
    if (!std::filesystem::exists(imgPath)) {
        if (!std::filesystem::create_directories(imgPath)) {
            std::cerr << "fail to create dir: " << imgPath << std::endl;
            exit(-1);
        }
    }
    isInited = true;
    initData();
    return true;
}

bool CRNNDataGenerator::initData() {
    getChemTexts();
    getDictTexts();
    std::cout << "chemTexts.size()=" << chemTexts.size() << ", dictTexts.size()="
              << dictTexts.size() << std::endl;
    getRandomTexts(10 * (chemTexts.size() + dictTexts.size()));
    if (availableFontFamilies.empty()) {
        auto allSystemFonts = QFontDatabase().families();
        static std::unordered_set<std::string> whiteList{
                "Arial", "Arial Narrow", "Arial Rounded MT Bold",

                "LM Mono 10", "LM Mono 12", "LM Mono 8", "LM Mono 9", "LM Mono Caps 10", "LM Mono Light 10",
                "LM Mono Light Cond 10", "LM Mono Prop 10", "LM Mono Prop Light 10", "LM Mono Slanted 10",
                "LM Roman 10",
                "LM Roman 12", "LM Roman 17", "LM Roman 5", "LM Roman 6", "LM Roman 7", "LM Roman 8", "LM Roman 9",
                "LM Roman Caps 10", "LM Roman Demi 10", "LM Roman Dunhill 10", "LM Roman Slanted 10",
                "LM Roman Slanted 12", "LM Roman Slanted 17", "LM Roman Slanted 8", "LM Roman Slanted 9",
                "LM Roman Unslanted 10", "LM Sans 10", "LM Sans 12", "LM Sans 17", "LM Sans 8", "LM Sans 9",
                "LM Sans Demi Cond 10", "LM Sans Quot 8",

                "Roboto", "Roboto Black", "Roboto Condensed", "Roboto Condensed Light", "Roboto Condensed Medium",
                "Roboto Light", "Roboto Medium", "Roboto Thin",

                "Ubuntu", "Ubuntu Condensed", "Ubuntu Light", "Ubuntu Mono", "Ubuntu Thin",

                "华文中宋", "华文仿宋", "华文宋体", "华文新魏", "华文楷体", "华文细黑", "幼圆", "文鼎ＰＬ简中楷", "文鼎ＰＬ简报宋",
                "方正姚体", "隶书"};
        for (auto &font: allSystemFonts) {
            if (!StdUtil::notExist(whiteList, font.toStdString())) {
                availableFontFamilies.push_back(font.toStdString());
            }
        }
    }
    isInited = true;
    return true;
}

void CRNNDataGenerator::dump(const size_t &_trainNum, const size_t &_testNum) {
    if (!isInited) {
        std::cerr << "you must call CRNNDataGenerator::init before dump data" << std::endl;
        exit(-1);
    }
//    auto env = lmdb::env::create();
//    env.set_mapsize(1UL * 1024UL * 1024UL * 1024UL * 1024UL); // 1 TB
//    env.open(topDir.c_str(), 0, 0664);
//    auto wtxn = lmdb::txn::begin(env);
//    auto dbi = lmdb::dbi::open(wtxn, nullptr);
    std::string text;
    int textType;
    std::string trainLabelFile = topDir + trainDir + gtFileName, testLabelFile = topDir + testDir + gtFileName;
    std::ofstream trainOfsm(trainLabelFile), testOfsm(testLabelFile);
//    dbi.put(wtxn, "num-samples", std::to_string(_trainNum).c_str());
    size_t blockSize = 1 + _trainNum / 20;
    for (size_t idx = 0; idx < _trainNum; idx++) {
        if (StdUtil::byProb(0.4)) { // 4:6
            text = StdUtil::randSelect(chemTexts);
            textType = 2;
        } else if (StdUtil::byProb(0.5)) { // 4:3:3
            text = StdUtil::randSelect(randomTexts);
            textType = 0;
        } else {
            text = StdUtil::randSelect(dictTexts);
            textType = 1;
        }
        auto[buffer, label]=getSample(text, textType);
        auto img = CvUtil::BufferToGrayMat(buffer);
        std::string filename = std::to_string(idx) + ".jpg";
        std::string file_path = topDir + trainDir + imgDir + filename;
        img.saveJPG(file_path);
        trainOfsm << filename << "\t" << label << "\n";
//        char a[100];
//        sprintf(a, imgKey, idx);
//        dbi.put(wtxn, a,"fuck");
//        dbi.put(wtxn, a, buffer.data(), buffer.size()+1, true);
//        sprintf(a, labelKey, idx);
//        dbi.put(wtxn, a, label.c_str(), label.length()+1, true);
//        dbi.put(wtxn, a,"fuck");
        if (idx % blockSize == blockSize - 1) {
//            wtxn.commit();
            std::cout << "idx=" << idx << std::endl;
        }
    }
    trainOfsm.close();
    for (size_t idx = 0; idx < _testNum; idx++) {
        if (StdUtil::byProb(0.6)) {
            text = StdUtil::randSelect(chemTexts);
            textType = 2;
        } else if (StdUtil::byProb(0.5)) {//五五开
            text = StdUtil::randSelect(randomTexts);
            textType = 0;
        } else {
            text = StdUtil::randSelect(dictTexts);
            textType = 1;
        }
        auto[buffer, label]=getSample(text, textType);
        auto img = CvUtil::BufferToGrayMat(buffer);
        std::string filename = std::to_string(idx) + ".jpg";
        std::string file_path = topDir + testDir + imgDir + filename;
        img.saveJPG(file_path);
        testOfsm << filename << "\t" << label << "\n";
//        char a[100];
//        sprintf(a, imgKey, idx);
//        dbi.put(wtxn, a,"fuck");
//        dbi.put(wtxn, a, buffer.data(), buffer.size()+1, true);
//        sprintf(a, labelKey, idx);
//        dbi.put(wtxn, a, label.c_str(), label.length()+1, true);
//        dbi.put(wtxn, a,"fuck");
        if (idx % (_testNum / 20) == _testNum / 20 - 1) {
//            wtxn.commit();
            std::cout << "idx=" << idx << std::endl;
        }
    }
    testOfsm.close();
//    wtxn.commit();
}

static std::vector<HwController> comm = {
        HwController(1),
        HwController(2),
};

std::pair<std::vector<uchar>, std::string> CRNNDataGenerator::getSample(
        const std::string &_text, int _type, bool _revertColor, bool _gaussianNoise, bool _saltNoise) {
    float k = StdUtil::betweenProb(1.5, 2.2);
    Mat img(MatChannel::GRAY, DataType::UINT8, width * k, height * k);
    if (StdUtil::byProb(0.5) && std::string::npos == _text.find("#") &&
        std::string::npos == _text.find("_") &&
        std::string::npos == _text.find("+")) {// 机打数据
        if (StdUtil::byProb(0.7)) {// 走 Qt 的富文本渲染
            auto rawText = CvUtil::GetFont(
                    _text, StdUtil::randSelect(availableFontFamilies));
            auto[tmpImg, _]= CvUtil::ResizeKeepRatio(rawText, img.getSize());
            img = std::move(tmpImg);
        } else {// 走 OpenCV 的常规字体渲染
            img.drawText(_text, {0, height * k / 1.2}, 1.5, ColorUtil::GetRGB(ColorName::rgbBlack), 1);
        }
    } else {// 手写数据
        HwStr hwStr;
        if (0 == _type) { // 随机字符
            hwStr.loadRichACSII(_text);
        } else if (1 == _type) {// 字典内容
            if (StdUtil::byProb(0.1)) {
                hwStr.loadRichACSII(_text);
            } else {
                hwStr.loadPlainText(_text);
            }
        } else {
            hwStr.loadRichACSII(_text);
        }
        hwStr.resizeTo(width * k - 4, height * k - 4, true);
        auto rect = hwStr.getBoundingBox().value();
        const auto[rW, rH]=getSize(rect);
        if (rW > width * k - 4) {
            hwStr.resizeTo(width * k - 4, height * k - 4, false);
        }
        if (StdUtil::byProb(0.2)) {// 均匀化间隔
            hwStr.equalize(width * k - 4);
        }
        hwStr.setHwController(comm[StdUtil::randInt() % comm.size()]);
        hwStr.moveLeftTopTo({1, 1});
        hwStr.paintTo(img);
    }
//    img.display("fuck");
    img = CvUtil::Resize(img, {width, height});
    if (_revertColor && StdUtil::byProb(0.5)) {// 反转颜色
        img = CvUtil::RevertColor(img);
    }
    if (_gaussianNoise && StdUtil::byProb(0.2)) {
        img = CvUtil::AddGaussianNoise(img);
    }
    if (_saltNoise && StdUtil::byProb(0.6)) {
        img = CvUtil::AddSaltPepperNoise(img, StdUtil::randInt() % 400);
    }

    return {img.toBuffer(), convertToKey(_text)};
}

void CRNNDataGenerator::display() {
    if (!isInited) {
        std::cerr << "you must call CRNNDataGenerator::init before dump data" << std::endl;
        exit(-1);
    }
    std::string text;
    int textType;
    for (size_t idx = 0; idx <= 500000; idx++) {
        if (StdUtil::byProb(0.4)) {//四六开
            text = StdUtil::randSelect(chemTexts);
            textType = 2;
        } else if (StdUtil::byProb(0.5)) {//五五开
            text = StdUtil::randSelect(randomTexts);
            textType = 0;
        } else {
            text = StdUtil::randSelect(dictTexts);
            textType = 1;
        }
        auto[buffer, label]=getSample(text, textType);
        std::cout << "********************\nlabel=" << label << std::endl;
        char a[100];
        sprintf(a, imgKey, idx);
        std::cout << "image-key=" << a << std::endl;
        sprintf(a, labelKey, idx);
        std::cout << "label-key=" << a << std::endl;
        auto img = CvUtil::BufferToGrayMat(buffer);
        img.display("CRNNDataGenerator::display");
    }
}

void CRNNDataGenerator::getDictTexts() {
    LineTextDataCreator dc;
    const std::string dict = DEV_ASSETS_DIR + std::string("/datasets/words_dictionary.json");
    dc.loadFromWordDict(dict.c_str());
    std::unordered_set<std::string> textSet;
    for (auto &text: dc.getWordSet()) {
        if (text.length() <= MAX_TEXT_LENGTH) {
            textSet.insert(text);
        }
    }
    dictTexts.clear();
    dictTexts.reserve(textSet.size());
    for (auto &text: textSet) {
        dictTexts.push_back(text);
    }
//    std::cout << textSet.size() << std::endl;
//    std::unordered_set<char> charSet;
//    for (auto &text:textSet) {
//        for (auto &c:text)charSet.insert(c);
//    }
//    for (auto &c:charSet) {
//        alphabet.push_back(c);
//    }
//    alphabet = convertToKey(alphabet);
//    charSet.clear();
//    for (auto &c:alphabet) {
//        charSet.insert(c);
//    }
//    alphabet.clear();
//    for (auto &c:charSet) {
//        alphabet.push_back(c);
//    }
//    std::sort(alphabet.begin(), alphabet.end());
//    std::cout << "alphabet=" << alphabet << std::endl;
}

std::string CRNNDataGenerator::convertToKey(const std::string &_key) const {
    std::string key;
    key.reserve(_key.size());
    for (auto &c: _key) {
        if (StdUtil::notExist(alphabetSet, c)) {
            if (std::isalpha(c)) {
                key.push_back(std::toupper(c));
            } else {
                std::cerr << "unhandled char: " << c << std::endl;
                exit(-1);
            }
        } else {
            key.push_back(c);
        }
    }
    return std::move(key);
}

void CRNNDataGenerator::getRandomTexts(const size_t &_num, const size_t &_len) {
    std::unordered_set<std::string> tmpTextSet;
    std::string tmp;
    tmp.resize(_len);
    while (tmpTextSet.size() < _num) {
        for (size_t i = 0; i < _len; i++) {
            tmp[i] = StdUtil::randSelect(SOSO_ALPHABET);
        }
        tmpTextSet.insert(tmp);
    }
    randomTexts.clear();
    for (auto &text: tmpTextSet) {
        randomTexts.push_back(text);
    }
}

void CRNNDataGenerator::getChemTexts() {
    LineTextDataCreator dc;
    dc.loadFromPattern(DEV_ASSETS_DIR + std::string("/../thirdparty/libopenbabel/data/openbabel/data/superatom.txt"));
    std::unordered_set<std::string> textSet;
    for (auto &text: dc.getWordSet()) {
        if (text.length() <= MAX_TEXT_LENGTH) {
            textSet.insert(text);
        }
    }
    chemTexts.clear();
    chemTexts.reserve(textSet.size());
    for (auto &text: textSet) {
        chemTexts.push_back(text);
    }
}

static std::vector<std::vector<std::string>> lGroupCandidates = {
        {"+NH4", "N+H4", "_S", "_O",  "_NH", "C_H2", "N_H", "_CH2", "_NH", "_CH2", "N+H4", "NH4+", "S_", "O_", "NH_", "CH2_", "N_H", "C_H2", "PH2", "OH", "SH", "NH2", "CO2Et", "COOEt", "CO2H", "CO2R", "COOR", "Et", "NCF3", "CF3", "CCl3", "CN", "NC", "N(OH)CH3", "NO2", "NO2", "NO", "SO3H", "COOH", "OEt", "OAc", "NHAc", "Ac", "CHO", "NMe", "SMe", "OMe", "COO_", "Ph", "OR", "NOH", "CO2Me", "COOMe", "SO2Me", "CBr3", "NHCbz", "SO2Cl", "COBr", "COBu", "COCF3", "COCl", "NHOH", "NMe2", "CH3", "C2H5",

                "+NH4", "N+H4", "_S", "_O", "_NH", "C_H2", "N_H", "_CH2", "_NH", "_CH2", "N+H4", "NH4+", "S_", "O_", "NH_", "CH2_", "N_H", "C_H2", "PH2", "OH", "SH", "NH2", "CO2Et", "COOEt", "CO2H", "CO2R", "COOR", "Et", "NCF3", "CF3", "CCl3", "CN", "NC", "N(OH)CH3", "NO2", "NO2", "NO", "SO3H", "COOH", "OEt", "OAc", "NHAc", "Ac", "CHO", "NMe", "SMe", "OMe", "COO_", "Ph", "OR", "NOH", "CO2Me", "COOMe", "SO2Me", "CBr3", "NHCbz", "SO2Cl", "COBr", "COBu", "COCF3", "COCl", "NHOH", "NMe2", "CH3", "C2H5",

                "+NH4", "N+H4", "_S", "_O", "_NH", "C_H2", "N_H", "_CH2", "_NH", "_CH2", "N+H4", "NH4+", "S_", "O_", "NH_", "CH2_", "N_H", "C_H2", "PH2", "OH", "SH", "NH2", "CO2Et", "COOEt", "CO2H", "CO2R", "COOR", "Et", "NCF3", "CF3", "CCl3", "CN", "NC", "N(OH)CH3", "NO2", "NO2", "NO", "SO3H", "COOH", "OEt", "OAc", "NHAc", "Ac", "CHO", "NMe", "SMe", "OMe", "COO_", "Ph", "OR", "NOH", "CO2Me", "COOMe", "SO2Me", "CBr3", "NHCbz", "SO2Cl", "COBr", "COBu", "COCF3", "COCl", "NHOH", "NMe2", "CH3", "C2H5",

                "+NH4", "N+H4", "_S", "_O", "_NH", "C_H2", "N_H", "_CH2", "_NH", "_CH2", "N+H4", "NH4+", "S_", "O_", "NH_", "CH2_", "N_H", "C_H2", "PH2", "OH", "SH", "NH2", "CO2Et", "COOEt", "CO2H", "CO2R", "COOR", "Et", "NCF3", "CF3", "CCl3", "CN", "NC", "N(OH)CH3", "NO2", "NO2", "NO", "SO3H", "COOH", "OEt", "OAc", "NHAc", "Ac", "CHO", "NMe", "SMe", "OMe", "COO_", "Ph", "OR", "NOH", "CO2Me", "COOMe", "SO2Me", "CBr3", "NHCbz", "SO2Cl", "COBr", "COBu", "COCF3", "COCl", "NHOH", "NMe2", "CH3", "C2H5",

                "OiBu", "tBu", "nBu", "iPr", "nPr", "OBz", "THPO", "NMe2", "NHZ", "CH2OH", "CH2NH2", "Me", "OBn", "NHAm", "OAm", "CO2Am", "COOAm", "SAm", "NHBn", "CO2Bn", "SnBu3", "NHBu", "OBu", "CO2Bu", "COOBu", "SBu", "COCO", "COEt", "COF", "COMe", "CONEt2", "CONH2", "CONHEt", "CONHMe", "CONMe2", "COSH", "NEt2", "NEt3", "NHEt", "SO2NH2", "OMs", "OCN", "SCN", "NHAm", "NHBn", "NHBu", "NHEt", "NHOH", "NHPr", "NO", "POEt2", "POEt3", "POOEt2", "PrNH", "SEt", "CH", "CH_", "D", "ND2", "DS", "Ace", "Acetyl", "Acyl", "AcAc", "Ad", "AOC", "Alloc", "Allyl", "All", "BDMS", "Boc", "BOC", "t-Boc", "BOM", "Bn", "Benzyl", "Bz", "Benzoyl", "Bromo", "n-Bu", "t-Bu", "cBu", "c-Bu", "c-But", "nButyl", "n-Butyl", "n-But", "t-Butyl", "cButyl", "c-Butyl", "Bs", "Cbz", "Z", "CE", "cinnamyl", "Chloro", "Dansyl", "Dan", "DEM", "DEIPS", "Dip", "Dmb", "DPA", "DPhAc", "DTBMS", "Et", "Ethyl", "EE", "EOM", "Fluoro", "Formyl", "FMOC", "nHeptyl", "n-Heptyl", "cHeptyl", "c-Heptyl", "nHexyl", "n-Hexyl", "cHexyl", "c-Hexyl", "Iodo", "IPDMS", "Methoxy", "MEM", "Mesityl", "MOM", "Mesyl", "Ms", "Nitro", "nOctyl", "n-Octyl", "n-Oct", "PAB", "nPentyl", "n-Pentyl", "cPentyl", "c-Pentyl", "Phenyl", "PMB", "Pv", "Pivaloyl", "Propargyl", "nPropyl", "n-Propyl", "n-Pr", "n-Pro", "cPropyl", "c-Propyl", "c-Pr", "c-Pro", "cPr", "i-Propyl", "i-Pr", "i-Pro", "SEM", "THP", "TMS", "TBDMS", "TBDPS", "TES", "Tf", "TIPS", "Tos", "p-Tos", "p-Tosyl", "4-Tosyl", "Tr", "Troc", "Vinyl", "Voc"},//1
        {"CH2",  "NH",   "N_", "C_H", "CH_", "NH2+", "N+H2"},//2
        {"CH",   "C_",   "C+", "NH+", "N+H"},//3
};
static std::vector<std::vector<std::string>> rGroupCandidates = {
        {
                "H4+N", "_S", "_O", "H_N", "H2_C", "H_N", "H2_C", "H4+N", "H4N+", "S_", "O_", "HN_", "H2C_", "HN_", "H2C_", "PH2", "OH", "SH", "_OOC", "H2N", "HO", "EtO2C", "EtOOC", "iBuO", "tBu", "nBu", "iPr", "nPr", "Et", "F3CN", "F3C", "Cl3C", "NC", "CN", "CH3(OH)N", "O2N", "O2N", "ON", "HO3S", "HOOC", "EtO", "AcO", "AcNH", "Ac", "OHC", "MeN", "MeS", "MeO", "Ph", "RO", "HON", "HO2C", "MeO2C", "MeOOC", "MeO2S", "Me2N", "RO2C", "ROOC", "ZNH", "HOCH2", "H2NCH2", "Me", "F3CCO", "ClCO", "COCO", "EtCO", "FCO", "MeCO", "Et2NCO", "H2NCO", "EtHNCO", "MeHNCO", "Me2NCO", "HSOC", "Et2N", "Et3N", "EtNH", "H2NSO2", "HONH", "Me2N",

                "H4+N", "_S", "_O", "H_N", "H2_C", "H_N", "H2_C", "H4+N", "H4N+", "S_", "O_", "HN_", "H2C_", "HN_", "H2C_", "PH2", "OH", "SH", "_OOC", "H2N", "HO", "EtO2C", "EtOOC", "iBuO", "tBu", "nBu", "iPr", "nPr", "Et", "F3CN", "F3C", "Cl3C", "NC", "CN", "CH3(OH)N", "O2N", "O2N", "ON", "HO3S", "HOOC", "EtO", "AcO", "AcNH", "Ac", "OHC", "MeN", "MeS", "MeO", "Ph", "RO", "HON", "HO2C", "MeO2C", "MeOOC", "MeO2S", "Me2N", "RO2C", "ROOC", "ZNH", "HOCH2", "H2NCH2", "Me", "F3CCO", "ClCO", "COCO", "EtCO", "FCO", "MeCO", "Et2NCO", "H2NCO", "EtHNCO", "MeHNCO", "Me2NCO", "HSOC", "Et2N", "Et3N", "EtNH", "H2NSO2", "HONH", "Me2N",

                "H4+N", "_S", "_O", "H_N", "H2_C", "H_N", "H2_C", "H4+N", "H4N+", "S_", "O_", "HN_", "H2C_", "HN_", "H2C_", "PH2", "OH", "SH", "_OOC", "H2N", "HO", "EtO2C", "EtOOC", "iBuO", "tBu", "nBu", "iPr", "nPr", "Et", "F3CN", "F3C", "Cl3C", "NC", "CN", "CH3(OH)N", "O2N", "O2N", "ON", "HO3S", "HOOC", "EtO", "AcO", "AcNH", "Ac", "OHC", "MeN", "MeS", "MeO", "Ph", "RO", "HON", "HO2C", "MeO2C", "MeOOC", "MeO2S", "Me2N", "RO2C", "ROOC", "ZNH", "HOCH2", "H2NCH2", "Me", "F3CCO", "ClCO", "COCO", "EtCO", "FCO", "MeCO", "Et2NCO", "H2NCO", "EtHNCO", "MeHNCO", "Me2NCO", "HSOC", "Et2N", "Et3N", "EtNH", "H2NSO2", "HONH", "Me2N",

                "H4+N", "_S", "_O", "H_N", "H2_C", "H_N", "H2_C", "H4+N", "H4N+", "S_", "O_", "HN_", "H2C_", "HN_", "H2C_", "PH2", "OH", "SH", "_OOC", "H2N", "HO", "EtO2C", "EtOOC", "iBuO", "tBu", "nBu", "iPr", "nPr", "Et", "F3CN", "F3C", "Cl3C", "NC", "CN", "CH3(OH)N", "O2N", "O2N", "ON", "HO3S", "HOOC", "EtO", "AcO", "AcNH", "Ac", "OHC", "MeN", "MeS", "MeO", "Ph", "RO", "HON", "HO2C", "MeO2C", "MeOOC", "MeO2S", "Me2N", "RO2C", "ROOC", "ZNH", "HOCH2", "H2NCH2", "Me", "F3CCO", "ClCO", "COCO", "EtCO", "FCO", "MeCO", "Et2NCO", "H2NCO", "EtHNCO", "MeHNCO", "Me2NCO", "HSOC", "Et2N", "Et3N", "EtNH", "H2NSO2", "HONH", "Me2N",

                "BzO", "THPO", "BnO", "AmNH", "AmO", "AmO2C", "AmOOC", "AmS", "BnNH", "BnO2C", "Bu3Sn", "BuNH", "BuO", "BuO2C", "BuOOC", "BuS", "Br3C", "CbzNH", "ClSO2", "BrCO", "BuCO", "MsO", "NCO", "NCS", "AmHN", "BnHN", "BuHN", "EtHB", "HOHN", "PrHN", "ON", "EtO2P", "EtO3P", "Et2OOP", "HNPr", "CCS", "HC", "HC_", "D", "D2N", "SD", "H3C", "C2H5", "Ace", "Acetyl", "Acyl", "AcAc", "Ad", "AOC", "Alloc", "Allyl", "All", "BDMS", "Boc", "BOC", "t-Boc", "BOM", "Bn", "Benzyl", "Bz", "Benzoyl", "Bromo", "n-Bu", "t-Bu", "cBu", "c-Bu", "c-But", "nButyl", "n-Butyl", "n-But", "t-Butyl", "cButyl", "c-Butyl", "Bs", "Cbz", "Z", "CE", "cinnamyl", "Chloro", "Dansyl", "Dan", "DEM", "DEIPS", "Dip", "Dmb", "DPA", "DPhAc", "DTBMS", "Et", "Ethyl", "EE", "EOM", "Fluoro", "Formyl", "FMOC", "nHeptyl", "n-Heptyl", "cHeptyl", "c-Heptyl", "nHexyl", "n-Hexyl", "cHexyl", "c-Hexyl", "Iodo", "IPDMS", "Methoxy", "MEM", "Mesityl", "MOM", "Mesyl", "Ms", "Nitro", "nOctyl", "n-Octyl", "n-Oct", "PAB", "nPentyl", "n-Pentyl", "cPentyl", "c-Pentyl", "Phenyl", "PMB", "Pv", "Pivaloyl", "Propargyl", "nPropyl", "n-Propyl", "n-Pr", "n-Pro", "cPropyl", "c-Propyl", "c-Pr", "c-Pro", "cPr", "i-Propyl", "i-Pr", "i-Pro", "SEM", "THP", "TMS", "TBDMS", "TBDPS", "TES", "Tf", "TIPS", "Tos", "p-Tos", "p-Tosyl", "4-Tosyl", "Tr", "Troc", "Vinyl", "Voc"},//1
        {       "H2C",  "HN", "N_", "H_C", "H2_N", "HC+", "H2N+"},//2
        {       "HC",   "C_", "C+", "HN+", "H+N"},//3
};

std::shared_ptr<HwBase> CRNNDataGenerator::getRectStr(const rectf &_freeRect, const int &_val, bool _isLeft) {
    if (_val <= 0 || _val > 3)
        return nullptr;
    std::string str;
    if (_isLeft) {
        str = StdUtil::randSelect(lGroupCandidates[StdUtil::randInt() % _val]);
    } else {
        str = StdUtil::randSelect(rGroupCandidates[StdUtil::randInt() % _val]);
    }
    auto hwStr = std::make_shared<HwStr>();
    hwStr->loadRichACSII(str);
    auto rect0 = hwStr->getBoundingBox().value();
    const auto[w0, h0]=getSize(rect0);
    float k0 = w0 / h0;
    const auto[w1, h1]=getSize(_freeRect);
    float k1 = w1 / h1;
    float height0 = h1;
    float width0 = (std::min)(w1, height0 * k0);
    if (k0 / 1.5 < k1) {// 允许水平1.5倍的压缩
        hwStr->resizeTo(width0, height0, false);
        const auto&[tl, br]=_freeRect;
        if (_isLeft) {
            hwStr->moveLeftTopTo(tl);
        } else {
            const auto&[x0, y0]=tl;
            const auto&[x1, y1]=br;
            hwStr->moveLeftTopTo({x1 - width0, y0});
        }
    } else {
        return nullptr;
    }
    return hwStr;
}

Mat CRNNDataGenerator::getStandardLongText() {
    std::string text;
    int textType;
    int MAX_LEN = 200 + StdUtil::randInt() % 400;
    int curLength = 0;
    int height0 = 18 + StdUtil::randInt() % 32;
    Mat result(MatChannel::GRAY, DataType::UINT8, 0, 0);
    while (curLength < MAX_LEN) {
        if (StdUtil::byProb(0.4)) {//四六开
            text = StdUtil::randSelect(chemTexts);
            textType = 2;
        } else if (StdUtil::byProb(0.5)) {//五五开
            text = StdUtil::randSelect(randomTexts);
            textType = 0;
        } else {
            text = StdUtil::randSelect(dictTexts);
            textType = 1;
        }
        auto img = CvUtil::GetFont(text, availableFontFamilies[StdUtil::randInt() % availableFontFamilies.size()]);
        img = CvUtil::Resize(img, {(float) height0 / img.getHeight() * img.getWidth(), height0});
        int originW = result.getWidth(), curW = img.getWidth();
        if (originW + curW < MAX_LEN) {
            if (result.getWidth() == 0) {
                result = img;
            } else {
                result = CvUtil::HConcat(result, img);
            }
        } else {
            break;
        }
        curLength = result.getWidth();
    }
    return result;
}
