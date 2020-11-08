#ifndef _COUCH_HPP_
#define _COUCH_HPP_

#include "config.hpp"
#include "shapeitem.hpp"
#include <map>

/**
 * You must call CouchLoader::LoadCouchDataSet()
 * before use any func below
 */
class CouchLoader {
private:
    inline static vector<vector<Script>> sData;
    // This map is used for test only. For common use, load config from filesystem.
    static vector<int> sLabelCharLikeLine;
    // This map is used for test only. For common use, load config from filesystem.
    static vector<int> sLabelCharLikeCircle;
    // This map is used for test only. For common use, load config from filesystem.
    static std::map<string, int> sStr2IntMap;
    inline static bool sIsLoaded = false;

    static void LoadDataSet(const char *filename, bool clearBefore = true);

public:
    static ShapeItem GetByIntLabel(int label);

    static ShapeItem GetByStrLabel(const string &label);

    static ShapeItem GetShape(const string &shape);

    static void LoadCouchDataSet();

    static const vector<vector<Script>> &GetData();

    static ShapeItem GetShapeItem(const size_t &_classIndex, const size_t &_sampleIndex);
};

#endif//_COUCH_HPP_