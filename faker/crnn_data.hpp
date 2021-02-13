#ifndef _CRNN_DATA_HPP_
#define _CRNN_DATA_HPP_
#include <string>

class CRNNDataGenerator {
    inline static const std::string dbName="soso-full.mdb";
    inline static const char* imgKey="image-%09d",*labelKey="label-%09d";
    inline static const int height=32,width=192;
    bool isInited;
    std::string topDir;
public:
    CRNNDataGenerator();

    bool init(const char *_topDir);

    void dump();
    void display();

};
#endif//_CRNN_DATA_HPP_
