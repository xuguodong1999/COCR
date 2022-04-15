#include "data/soso_crnn.h"


#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <QPainter>
#include <QTextDocument>
#include <QFontDatabase>
#include <QDebug>

#include <filesystem>
#include <fstream>
#include <iostream>

static std::vector<QString> availableFontFamilies = {"Arial"};

static cv::Mat GetFont(const QString &_text, const QString &_fontFamily = "Arial") {
//    return cv::Mat(32,32,CV_8UC1,cv::Scalar(0));
    QFont font;
    font.setFamily(_fontFamily);
    font.setWeight(1);
    font.setItalic(byProb(0.5));

    QImage image(1280, 164, QImage::Format_Grayscale8);
    image.fill(Qt::white);
    QPainter painter(&image);
    painter.setFont(font);
    QTextDocument td;
    td.setDefaultFont(font);
    td.setHtml(_text);
    float k = 5;
    painter.scale(k, k);
    painter.translate(0, 0);
    // FIXME: why QTextDocument.drawContents cant run as parallel job through openmp?
    td.drawContents(&painter);
    cv::Mat cvImg(image.height(), image.width(),
                  CV_8UC1, (void *) image.constBits(), image.bytesPerLine());
    auto rectPtr = getBoundBoxForBWFont(cvImg);
    if (rectPtr) {
        cvImg = cvImg(cv::Rect(rectPtr.value()));
    }
//    cv::imshow("1",cvImg);
//    cv::waitKey(0);
    return cvImg.clone();
}

std::string SOSO_ALPHABET = "-=#+_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabdefghijnqrty";

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
        static QSet<QString> whiteList = {
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
            if (whiteList.contains(font)) {
                availableFontFamilies.push_back(font);
            }
        }
    }
    isInited = true;
    return true;
}

static const std::vector<int> fontChoices = {
        cv::FONT_HERSHEY_SIMPLEX, cv::FONT_HERSHEY_DUPLEX, cv::FONT_HERSHEY_COMPLEX,
        cv::FONT_HERSHEY_TRIPLEX};

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
    for (size_t idx = 0; idx < _trainNum; idx++) {
        if (byProb(0.4)) { // 4:6
            text = randSelect(chemTexts);
            textType = 2;
        } else if (byProb(0.5)) { // 4:3:3
            text = randSelect(randomTexts);
            textType = 0;
        } else {
            text = randSelect(dictTexts);
            textType = 1;
        }
        auto[buffer, label]=getSample(text, textType);
        auto img = cv::imdecode(buffer, cv::IMREAD_GRAYSCALE);
        std::string filename = std::to_string(idx) + ".jpg";
        std::string file_path = topDir + trainDir + imgDir + filename;
        cv::imwrite(file_path, img, {cv::IMWRITE_JPEG_QUALITY, 70 + randInt() % 30});
        trainOfsm << filename << "\t" << label << "\n";
//        char a[100];
//        sprintf(a, imgKey, idx);
//        dbi.put(wtxn, a,"fuck");
//        dbi.put(wtxn, a, buffer.data(), buffer.size()+1, true);
//        sprintf(a, labelKey, idx);
//        dbi.put(wtxn, a, label.c_str(), label.length()+1, true);
//        dbi.put(wtxn, a,"fuck");
        if (idx % (_trainNum / 20) == _trainNum / 20 - 1) {
//            wtxn.commit();
            std::cout << "idx=" << idx << std::endl;
        }
    }
    trainOfsm.close();
    for (size_t idx = 0; idx < _testNum; idx++) {
        if (byProb(0.6)) {
            text = randSelect(chemTexts);
            textType = 2;
        } else if (byProb(0.5)) {//五五开
            text = randSelect(randomTexts);
            textType = 0;
        } else {
            text = randSelect(dictTexts);
            textType = 1;
        }
        auto[buffer, label]=getSample(text, textType);
        auto img = cv::imdecode(buffer, cv::IMREAD_GRAYSCALE);
        std::string filename = std::to_string(idx) + ".jpg";
        std::string file_path = topDir + testDir + imgDir + filename;
        cv::imwrite(file_path, img, {cv::IMWRITE_JPEG_QUALITY, 60 + randInt() % 40});
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
    float k = betweenProb(1.5, 2.2);
    cv::Mat img = cv::Mat(height * k, width * k, CV_8UC3,
                          getScalar(ColorName::rgbWhite));
    if (byProb(0.5) && std::string::npos == _text.find("#") &&
        std::string::npos == _text.find("_") &&
        std::string::npos == _text.find("+")) {// 机打数据
        if (byProb(0.7)) {// 走 Qt 的富文本渲染
            QString qData;
            for (auto &c: _text) {
                if ('0' <= c && c <= '9') {
                    qData.push_back(QString("<sub>") + c + "</sub>");
                } else {
                    qData.append(c);
                }
            }
            auto rawText = GetFont(qData, availableFontFamilies[randInt() % availableFontFamilies.size()]);
            cv::cvtColor(rawText, rawText, cv::COLOR_GRAY2BGR);
            auto[img2, _]=resizeCvMatTo(rawText, img.cols, img.rows);
            img = img2;
        } else {// 走 OpenCV 的常规字体渲染
            cv::putText(img, _text, cv::Point(0, height * k / 1.25),
                        randSelect(fontChoices), 1.5, getScalar(ColorName::rgbBlack),
                        1, cv::LINE_AA, false);
        }
    } else {// 手写数据
        HwStr hwStr;
        if (0 == _type) {// 随机字符
            hwStr.loadRichACSII(_text);
        } else if (1 == _type) {// 字典内容
            if (byProb(0.1)) {
                hwStr.loadRichACSII(_text);
            } else {
                hwStr.loadPlainText(_text);
            }
        } else {
            hwStr.loadRichACSII(_text);
        }
        hwStr.resizeTo(width * k - 4, height * k - 4, true);
        auto rect = hwStr.getBoundingBox().value();
        if (rect.width > width * k - 4) {
            hwStr.resizeTo(width * k - 4, height * k - 4, false);
        }
        if (byProb(0.2)) {// 均匀化间隔
            hwStr.equalize(width * k - 4);
        }
        hwStr.setHwController(comm[randInt() % comm.size()]);
        hwStr.moveLeftTopTo(cv::Point2f(1, 1));
        hwStr.paintTo(img);
    }
    cv::resize(img, img, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
    if (_revertColor && byProb(0.5)) {// 反转颜色
        cv::bitwise_not(img, img);
    }
    img.convertTo(img, CV_32F, 1. / 255);
    if (_gaussianNoise && byProb(0.2)) {
        cv::Mat noise(img.size(), CV_32FC3);
        cv::randn(noise, 0, belowProb(0.1));
        img = img + noise;
        cv::normalize(img, img, 1.0, 0, cv::NORM_MINMAX, CV_32F);
    }
    img.convertTo(img, CV_8U, 255);
    if (_saltNoise && byProb(0.6)) {
        salt_pepper(img, randInt() % 400);
    }
    std::vector<uchar> buffer;
    cv::imencode(".jpg", img, buffer, {cv::IMWRITE_JPEG_QUALITY, 100});
    return {std::move(buffer), std::move(convertToKey(_text))};
}

void CRNNDataGenerator::display() {
    if (!isInited) {
        std::cerr << "you must call CRNNDataGenerator::init before dump data" << std::endl;
        exit(-1);
    }
    std::string text;
    int textType;
    for (size_t idx = 0; idx <= 500000; idx++) {
        if (byProb(0.4)) {//四六开
            text = randSelect(chemTexts);
            textType = 2;
        } else if (byProb(0.5)) {//五五开
            text = randSelect(randomTexts);
            textType = 0;
        } else {
            text = randSelect(dictTexts);
            textType = 1;
        }
        auto[buffer, label]=getSample(text, textType);
        std::cout << "********************\nlabel=" << label << std::endl;
        char a[100];
        sprintf(a, imgKey, idx);
        std::cout << "image-key=" << a << std::endl;
        sprintf(a, labelKey, idx);
        std::cout << "label-key=" << a << std::endl;
        auto img = cv::imdecode(buffer, cv::IMREAD_GRAYSCALE);
        cv::imshow("fuck", img);
        cv::waitKey(0);
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
        if (notExist(alphabetSet, c)) {
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
            tmp[i] = randSelect(SOSO_ALPHABET);
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
    dc.loadFromPattern(DEV_ASSETS_DIR + std::string("/openbabel/data/superatom.txt"));
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

std::shared_ptr<HwBase> CRNNDataGenerator::getRectStr(const cv::Rect2f &_freeRect, const int &_val, bool _isLeft) {
    if (_val <= 0 || _val > 3)
        return nullptr;
    std::string str;
    if (_isLeft) {
        str = randSelect(lGroupCandidates[randInt() % _val]);
    } else {
        str = randSelect(rGroupCandidates[randInt() % _val]);
    }
    auto hwStr = std::make_shared<HwStr>();
    hwStr->loadRichACSII(str);
    auto rect0 = hwStr->getBoundingBox().value();
    float k0 = rect0.width / rect0.height;
    float k1 = _freeRect.width / _freeRect.height;
    float height = _freeRect.height;
    float width = (std::min)(_freeRect.width, height * k0);
    if (k0 / 1.5 < k1) {// 允许水平1.5倍的压缩
        hwStr->resizeTo(width, height, false);
        if (_isLeft) {
            hwStr->moveLeftTopTo(_freeRect.tl());
        } else {
            hwStr->moveLeftTopTo(cv::Point2f(_freeRect.x + _freeRect.width - width, _freeRect.y));
        }
    } else {
        return nullptr;
    }
    return hwStr;
}

cv::Mat CRNNDataGenerator::getStandardLongText() {
    std::string text;
    int textType;
    int MAX_LEN = 200 + randInt() % 400;
    int curLength = 0;
    int height = 18 + randInt() % 32;
    cv::Mat result;
    while (curLength < MAX_LEN) {
        if (byProb(0.4)) {//四六开
            text = randSelect(chemTexts);
            textType = 2;
        } else if (byProb(0.5)) {//五五开
            text = randSelect(randomTexts);
            textType = 0;
        } else {
            text = randSelect(dictTexts);
            textType = 1;
        }
        QString qData;
        for (auto &c: text) {
            if ('0' <= c && c <= '9') {
                qData.push_back(QString("<sub>") + c + "</sub>");
            } else if ('#' == c) {
                qData.append("≡");
            } else if ('+' == c) {
                if (byProb(0.5))
                    qData.append(QString("<sup>+</sup>"));
                else
                    qData.append(QString("<sup>⊕</sup>"));
            } else if ('-' == c) {
                if (byProb(0.5))
                    qData.append(QString("<sup>-</sup>"));
                else
                    qData.append(QString("<sup>㊀</sup>"));
            } else {
                qData.append(c);
            }
        }
        auto img = GetFont(qData, availableFontFamilies[randInt() % availableFontFamilies.size()]);
        cv::resize(img, img, cv::Size(((float) height / img.rows * img.cols), height),
                   0, 0, cv::INTER_CUBIC);
        int originW = result.cols, curW = img.cols;
        if (originW + curW < MAX_LEN) {
            if (result.empty()) {
                result = img;
            } else {
                cv::hconcat(result, img, result);
            }
        } else {
            break;
        }
        curLength = result.cols;
    }
    if (!result.empty())
        cv::cvtColor(result, result, cv::COLOR_GRAY2BGR);
    return result;
}
