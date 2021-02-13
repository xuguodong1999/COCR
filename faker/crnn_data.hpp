#ifndef _CRNN_DATA_HPP_
#define _CRNN_DATA_HPP_
#include <string>

class CRNNDataGenerator {
    inline static const std::string imgDir = "/JPEGImages/", labelDir = "/labels/";
    bool isInited;
    std::string imgPath, labelPath;
public:
    CRNNDataGenerator();

    bool init(const char *_topDir);

    void dump(const size_t &_numOfSamples, const size_t &_repeatTimes = 1);
};
#endif//_CRNN_DATA_HPP_
