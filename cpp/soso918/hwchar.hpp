//
// Created by xgd on 2020/9/18.
//

#ifndef PLAY_OPENCV_HWCHAR_HPP
#define PLAY_OPENCV_HWCHAR_HPP

#include "shape.hpp"
#include "utils.hpp"
#include <map>
#include <vector>
#include <iostream>

/**
 * Hand-Written Char
 */
class HWChar : public Shape {
    int mLabel;
    // Script[标签][数据量]
    static std::vector<std::vector<Script>> sData;
    static std::vector<int> sLabelCharLikeLine;
    static std::vector<int> sLabelCharLikeCircle;
    static std::map<std::string, int> sStr2IntMap;
    static bool sIsLoaded;

    static bool LoadDataSet(const char *filename, bool clearBefore = true);

    HWChar(int label);

public:
    static HWChar GetByIntLabel(int label) {
        return HWChar(label);
    }

    static HWChar GetByStrLabel(const std::string &label) {
        auto resItr = sStr2IntMap.find(label);
        if (resItr == sStr2IntMap.end()) {
            std::cerr << "fail to find " << label
                      << "in HWChar::sStr2IntMap" << std::endl;
            exit(-1);
        }
        return HWChar(resItr->second);
    }

    static HWChar GetShape(const std::string &shape) {
        if (shape == "line") {
            return HWChar(
                    sLabelCharLikeLine[rand() % sLabelCharLikeLine.size()]);
        } else if (shape == "circle") {
            auto tmp = HWChar(
                    sLabelCharLikeCircle[rand() % sLabelCharLikeCircle.size()]);
            tmp.castToCircle(cv::Point2f(50, 50), 50, 50);
            return tmp;
        } else {
            std::cerr << shape << " not supported! "
                      << "Only \"line\" and \"circle\" supported!" << std::endl;
            exit(-1);
        }
    }
};

#endif //PLAY_OPENCV_HWCHAR_HPP
