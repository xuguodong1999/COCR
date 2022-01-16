#ifndef _DARKNET_DATA_HPP_
#define _DARKNET_DATA_HPP_

#include <string>

class SOSODarknet {
    inline static const std::string imgDir = "/JPEGImages/", labelDir = "/labels/";
    bool isInited;
    std::string imgPath, labelPath;
public:
    SOSODarknet();

    bool init(const std::string &_topDir);

    void dump(const size_t &_numOfSamples, const size_t &_repeatTimes = 5);

    void display();
};

#endif//_DARKNET_DATA_HPP_
