///**
// * FIXME: This file is shit. Dont base new codes on it.
// */
//#ifndef _COUCH_DATA_HPP_
//#define _COUCH_DATA_HPP_
//
//#include "hw.hpp"
//#include <map>
//#include <string>
//#include <vector>
//
///**
// * You must call CouchLoader::LoadCouchDataSet()
// * before use any func below
// */
//class CouchLoader {
//private:
//    inline static std::vector<std::vector<Script>> sData;
//    // This map is used for test only. For common use, load config from filesystem.
//    static std::vector<int> sLabelCharLikeLine;
//    // This map is used for test only. For common use, load config from filesystem.
//    static std::vector<int> sLabelCharLikeCircle;
//    // This map is used for test only. For common use, load config from filesystem.
//    static std::map<std::string, int> sStr2IntMap;
//    inline static bool sIsLoaded = false;
//
//    static void LoadDataSet(const char *filename, bool _fromQrc = false,
//                            bool clearBefore = true);
//
//public:
//    static ShapeItem GetByIntLabel(int label);
//
//    static ShapeItem GetByStrLabel(const std::string &label);
//
//    static ShapeItem GetShape(const std::string &shape);
//
//    static void LoadCouchDataSet(bool _fromQrc = false);
//
//    static const std::vector<std::vector<Script>> &GetData();
//
//    static ShapeItem GetShapeItem(const size_t &_classIndex, const size_t &_sampleIndex);
//};
//
//#endif//_COUCH_DATA_HPP_
