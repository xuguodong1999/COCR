#ifndef _CRNN_DATA_HPP_
#define _CRNN_DATA_HPP_

#include <string>
#include <vector>
#include <unordered_set>

class CRNNDataGenerator {
    inline static const char *imgKey = "image-%09d", *labelKey = "label-%09d";
    inline static const std::string imgDir = "JPEGImages", gtFileName = "labels.txt";
    inline static const int height = 32, width = 192;
    inline static const size_t MAX_TEXT_LENGTH = 7;
    bool isInited;
    std::string topDir;
    std::vector<std::string> randomTexts, chemTexts, dictTexts;
    std::string alphabet;
    inline static std::string std_alphabet = "-=#+_()0123456789ABDEFGHLNQRTYabcdefghijklmnopqrstuvwxyz";
    inline static std::unordered_set<char> std_alphabet_set;

    std::string convertToKey(const std::string &_key) const;

    /**
     *
     * @param _text
     * @param _type 0: 随机字符；1: 字典内容; 2: 化学符号
     * @return
     */
    std::pair<std::vector<unsigned char>, std::string> getSample(
            const std::string &_text,int _type=0);

public:
    CRNNDataGenerator();

    void getRandomTexts(const size_t &_num, const size_t &_len = MAX_TEXT_LENGTH);

    bool init(const char *_topDir);

    void getDictTexts();

    void getChemTexts();

    void dump(const size_t& SAMPLE_NUM=500000);

    void display();


};
#endif//_CRNN_DATA_HPP_
