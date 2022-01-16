#ifndef _COUCH_HPP_
#define _COUCH_HPP_

#include "config.hpp"
#include "shapeitem.hpp"
#include <map>

class CouchItem {
private:
    static vector<vector<Script>> sData;
    static vector<int> sLabelCharLikeLine;
    static vector<int> sLabelCharLikeCircle;
    static std::map<string, int> sStr2IntMap;
    static bool sIsLoaded;

    static void LoadDataSet(const char *filename, bool clearBefore = true);

public:
    static ShapeItem GetByIntLabel(int label);

    static ShapeItem GetByStrLabel(const string &label);

    static ShapeItem GetShape(const string &shape);
};

#endif//_COUCH_HPP_