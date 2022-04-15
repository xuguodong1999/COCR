#pragma once

#include "els_data_export.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

extern std::string SOSO_ALPHABET;

class ELS_DATA_EXPORT CRNNDataGenerator {
    inline static const char *imgKey = "image-%09d";
    inline static const char *labelKey = "label-%09d";
    inline static const std::string imgDir = "/JPEGImages/";
    inline static const std::string gtFileName = "labels.txt";
    inline static const std::string trainDir = "/train/";
    inline static const std::string testDir = "/test/";
    inline static const int height = 32;
    inline static const int width = 192;
    inline static const size_t MAX_TEXT_LENGTH = 7;
    bool isInited;
    std::string topDir;
    std::vector<std::string> randomTexts, chemTexts, dictTexts;

    std::unordered_set<char> alphabetSet;

    std::string convertToKey(const std::string &_key) const;

    /**
     *
     * @param _text
     * @param _type 0: 随机字符；1: 字典内容; 2: 化学符号
     * @return
     */
    std::pair<std::vector<unsigned char>, std::string> getSample(
            const std::string &_text, int _type = 0, bool _revertColor = true, bool _gaussianNoise = true,
            bool _saltNoise = true);

public:
    CRNNDataGenerator();

    void getRandomTexts(const size_t &_num, const size_t &_len = MAX_TEXT_LENGTH);

    bool init(const std::string &_topDir);

    bool initData();

    void getDictTexts();


    void getChemTexts();

    void dump(const size_t &_trainNum = 500000, const size_t &_testNum = 10000);

    void display();

    /**
     * 返回一个可以填入 freeRect 的字符串图元
     * @param _freeRect
     * @param _isLeft 自由基在左侧
     * @return
     */
    std::shared_ptr<HwBase> getRectStr(const cv::Rect2f &_freeRect, const int &_val, bool _isLeft);

    cv::Mat getStandardLongText();
};

ELS_DATA_EXPORT extern CRNNDataGenerator crnnDataGenerator;