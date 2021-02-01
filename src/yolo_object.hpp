#ifndef _GT_BOX_HPP_
#define _GT_BOX_HPP_

#include <vector>

struct YoloObject {
    float x, y, w, h, prob;
    int label;

    YoloObject(const float &_x, const float &_y, const float &_w, const float &_h,
               const int &_label, const float &_prob = 1) :
            x(_x), y(_y), w(_w), h(_h), label(_label), prob(_prob) {}
};

inline float getAvgObjectSize(const std::vector<YoloObject> &_objs) {
    if (_objs.empty())return -1;
    float avg = 0;
    for (auto &obj:_objs)avg += std::max(obj.w, obj.h);
    return avg / _objs.size();
}

#endif
