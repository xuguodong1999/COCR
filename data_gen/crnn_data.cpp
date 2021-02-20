#include "crnn_data.hpp"
#include "hw_str.hpp"
#include "linetextdata.hpp"
#include "colors.hpp"
#include "std_util.hpp"

//#include "lmdb++.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

std::string SOSO_ALPHABET = "-=#+_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabdefghijnqrty";

CRNNDataGenerator::CRNNDataGenerator() : isInited(false) {
    alphabetSet.clear();
    for (auto &c:SOSO_ALPHABET) {
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
    getChemTexts();
    getDictTexts();
    std::cout << "chemTexts.size()=" << chemTexts.size() << ", dictTexts.size()="
              << dictTexts.size() << std::endl;
    getRandomTexts(chemTexts.size() + dictTexts.size());
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
        auto img = cv::imdecode(buffer, cv::IMREAD_GRAYSCALE);
        std::string filename = std::to_string(idx) + ".jpg";
        std::string file_path = topDir + trainDir + imgDir + filename;
        cv::imwrite(file_path, img, {cv::IMWRITE_JPEG_QUALITY, 70 + rand() % 30});
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
        auto img = cv::imdecode(buffer, cv::IMREAD_GRAYSCALE);
        std::string filename = std::to_string(idx) + ".jpg";
        std::string file_path = topDir + testDir + imgDir + filename;
        cv::imwrite(file_path, img, {cv::IMWRITE_JPEG_QUALITY, 70 + rand() % 30});
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

std::pair<std::vector<uchar>, std::string> CRNNDataGenerator::getSample(
        const std::string &_text, int _type) {
    float k = betweenProb(1.5, 2.2);
    cv::Mat img = cv::Mat(height * k, width * k, CV_8UC3,
                          getScalar(ColorName::rgbWhite));
    if (byProb(0.5) && std::string::npos == _text.find("#") &&
        std::string::npos == _text.find("_") &&
        std::string::npos == _text.find("+")) {// 机打数据
        cv::putText(img, _text, cv::Point(0, height * k / 1.25),
                    randSelect(fontChoices), 1.5, getScalar(ColorName::rgbBlack),
                    1, cv::LINE_AA, false);
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
        HwController hwController(2);
        hwStr.setHwController(hwController);
        hwStr.moveLeftTopTo(cv::Point2f(1, 1));
        hwStr.paintTo(img);
    }
    cv::resize(img, img, cv::Size(width, height), 0, 0, cv::INTER_CUBIC);
    if (byProb(0.5)) {// 反转颜色
        cv::bitwise_not(img, img);
    }
    img.convertTo(img, CV_32F, 1. / 255);
    if (byProb(0.2)) {
        cv::Mat noise(img.size(), CV_32FC3);
        cv::randn(noise, 0, belowProb(0.1));
        img = img + noise;
        cv::normalize(img, img, 1.0, 0, cv::NORM_MINMAX, CV_32F);
    }
    img.convertTo(img, CV_8U, 255);
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
    dc.loadFromWordDict();
    std::unordered_set<std::string> textSet;
    for (auto &text:dc.getWordSet()) {
        if (text.length() <= MAX_TEXT_LENGTH) {
            textSet.insert(text);
        }
    }
    dictTexts.clear();
    dictTexts.reserve(textSet.size());
    for (auto &text:textSet) {
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
    for (auto &c:_key) {
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
    randomTexts.reserve(tmpTextSet.size());
    for (auto &text:tmpTextSet) {
        randomTexts.push_back(text);
    }
}

void CRNNDataGenerator::getChemTexts() {
    LineTextDataCreator dc;
    dc.loadFromPattern();
    std::unordered_set<std::string> textSet;
    for (auto &text:dc.getWordSet()) {
        if (text.length() <= MAX_TEXT_LENGTH) {
            textSet.insert(text);
        }
    }
    chemTexts.clear();
    chemTexts.reserve(textSet.size());
    for (auto &text:textSet) {
        chemTexts.push_back(text);
    }
}

static std::vector<std::vector<std::string>> lGroupCandidates = {
        {"CH3", "NH2", "OH"},//1
        {"CH2", "NH"},//2
        {"CH"},//3
};
static std::vector<std::vector<std::string>> rGroupCandidates = {
        {"H3C", "H2N", "HO"},//1
        {"H2C", "HN"},//2
        {"HC"},//3
};

std::shared_ptr<HwBase> CRNNDataGenerator::getRectStr(const cv::Rect &_freeRect, const int &_val, bool _isLeft) {
    if (_val <= 0 || _val > 3)
        return nullptr;
    std::string str;
    if (_isLeft) {
        str = randSelect(lGroupCandidates[_val - 1]);
    } else {
        str = randSelect(rGroupCandidates[_val - 1]);
    }
    auto hwStr = std::make_shared<HwStr>();
    hwStr->loadRichACSII(str);
    auto rect0 = hwStr->getBoundingBox().value();
    float k0 = rect0.width / rect0.height;
    float k1 = _freeRect.width / _freeRect.height;
    float height = _freeRect.height;
    float width = (std::min)(rect0.width, height * k0);
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
