#ifndef _HANDWRITTING_HPP_
#define _HANDWRITTING_HPP_

#include "config.hpp"
#include "shape.hpp"
#include <map>

class HandWritting {
private:
    static vector<vector<Script>> sData;
    static vector<int> sLabelCharLikeLine;
    static vector<int> sLabelCharLikeCircle;
    static std::map<string, int> sStr2IntMap;
    static bool sIsLoaded;

    static void LoadDataSet(const char *filename, bool clearBefore = true);

public:
    static Shape GetByIntLabel(int label);

    static Shape GetByStrLabel(const string &label);

    static Shape GetShape(const string &shape);
};

#endif//_HANDWRITTING_HPP_