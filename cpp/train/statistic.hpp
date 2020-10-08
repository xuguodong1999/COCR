#ifndef _STATISTIC_HPP_
#define _STATISTIC_HPP_

#include "config.hpp"
#include <set>

/**
 * randomization controller
 * 意图：1、插桩执行数据分布统计；2、控制随机性
 */
class RC {
public:
    static float sNormSizeK;    // (-1,1)*100
    static float sSubSizeK;     // (-1,1)*50
    static float sNormOffsetKx; // (-1,1)*100
    static float sNormOffsetKy; // (-1,1)*100
    static float sSubOffsetKx;  // (-1,1)*50
    static float sSubOffsetKy;  // (-1,1)*50
    static float sAngleK;       // [0,1)*30
    static v<s> sElementData;
    static std::set<s> bsSet, aeSet, btSet;
    static std::set<int> acSet;
};

#endif//_STATISTIC_HPP_