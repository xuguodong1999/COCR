#pragma once

#include "xgd_data_export.h"

#include "XGD/Vision/Mat.h"

#include <string>
#include <vector>
#include <memory>
#include <unordered_set>

class HwBase;

class XGD_DATA_EXPORT CRNNDataGenerator {
    inline static std::string SOSO_ALPHABET = "-=#+_()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabdefghijnqrty";
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
     * @param _type 0: random texts 1: texts from dictionary  2: chemical words
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
     * @param _freeRect
     * @param _isLeft expand to left?
     * @return a string item that can be filled into _freeRect
     */
    std::shared_ptr<HwBase> getRectStr(const rectf &_freeRect, const int &_val, bool _isLeft);

    Mat getStandardLongText();
};

XGD_DATA_EXPORT extern CRNNDataGenerator crnnDataGenerator;