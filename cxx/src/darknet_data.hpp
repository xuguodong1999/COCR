#ifndef _DARKNET_DATA_HPP_
#define _DARKNET_DATA_HPP_

#include <string>

class DarknetDataGenerator {
    inline static const std::string imgDir = "/JPEGImages/", labelDir = "/labels/";
    bool isInited;
    std::string imgPath, labelPath;
public:
    DarknetDataGenerator();

    bool init(const char *_topDir);

    void dump(const size_t &_numOfSamples, const size_t &_repeatTimes = 1);
};

#endif//_DARKNET_DATA_HPP_
