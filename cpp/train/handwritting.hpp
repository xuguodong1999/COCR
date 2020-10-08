#ifndef _HANDWRITTING_HPP_
#define _HANDWRITTING_HPP_

#include "config.hpp"
#include "shape.hpp"
#include <map>

class HandWritting {
private:
    static v<v<Script>> sData;
    static v<int> sLabelCharLikeLine;
    static v<int> sLabelCharLikeCircle;
    static std::map<s, int> sStr2IntMap;
    static bool sIsLoaded;

    static void LoadDataSet(const char *filename, bool clearBefore = true);

public:
    static Shape GetByIntLabel(int label);

    static Shape GetByStrLabel(const std::string &label);

    static Shape GetShape(const std::string &shape);
};

#endif//_HANDWRITTING_HPP_