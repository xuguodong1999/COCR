#ifndef _YOLOV3_H
#define _YOLOV3_H

#include <algorithm>
#include <cassert>
#include <utility>
#include <vector>
#include <list>
#include <QRectF>

using namespace std;
namespace cocr {
    class Object {
    private:
        float x, y, w, h, conf;
        int index;
    public:
        Object() {}

        Object(float x, float y, float w, float h, float conf, int index)
                : x(x), y(y), w(w), h(h), conf(conf), index(index) {}

        QRectF toQRectF() {
            return QRectF(x - w / 2, y - h / 2, w, h);
        }

        friend std::ostream &operator<<(std::ostream &out, const Object &o) {
            out << "\n[" << o.x << "," << o.y << "," << o.w << "," << o.h << "," << o.conf << "," << o.index << "]";
            return out;
        }

        int getIndex() const {
            return index;
        }

        void setIndex(int index) {
            Object::index = index;
        }

        float getX() const {
            return x;
        }

        void setX(float x) {
            Object::x = x;
        }

        float getY() const {
            return y;
        }

        void setY(float y) {
            Object::y = y;
        }

        float getW() const {
            return w;
        }

        void setW(float w) {
            Object::w = w;
        }

        float getH() const {
            return h;
        }

        void setH(float h) {
            Object::h = h;
        }

        float getConf() const {
            return conf;
        }

        void setConf(float conf) {
            Object::conf = conf;
        }

        float iou(const Object &o) const {
            float x0 = x - w / 2, y0 = y - h / 2,
                    x1 = x + w / 2, y1 = y + h / 2;
            float x2 = o.x - o.w / 2, y2 = o.y - o.h / 2,
                    x3 = o.x + o.w / 2, y3 = o.y + o.h / 2;
            float ww = std::min(x1, x3) - std::max(x0, x2);
            float hh = std::min(y1, y3) - std::max(y0, y2);
            if (ww <= 0 || hh <= 0) {
                return 0;
            }
            float sa = w * h, sb = o.w * o.h, ss = ww * hh;
            return ss / (sa + sb - ss);
        }

        friend inline void nms(list<Object> &objects,
                               const float &conf_thresh = 0.5, const float &iou_thresh = 0.5) {
            for (auto it = objects.begin(); it != objects.end();) {
                if (it->conf < conf_thresh) {
                    it = objects.erase(it);
                } else {
                    it++;
                }
            }
            objects.sort([](const Object &a, const Object &b) {
                return a.conf > b.conf;
            });
            for (auto it1 = objects.cbegin(); it1 != objects.end(); it1++) {
                auto it2 = it1;
                it2++;
                while (it2 != objects.end()) {
                    if (it1->iou(*it2) > iou_thresh) {
                        it2 = objects.erase(it2);
                    } else {
                        it2++;
                    }
                }
            }
        }
    };

    class Yolov3Layer {
    public:
        /*
         *         int classes;
        int how_many_scales;// 通常是在2/3个尺度下检测
        vector<pair<float, float>> anchors;
        int how_many_anchors_per_grid;// 支持更多anchor数目（yolov3及其变种几乎都使用3）
        int max_grid_width, max_grid_height;// 支持矩形grid（yolov3及其变种几乎都使用32x32）
         */
        void log() {
            LogName(classes);
            LogName(how_many_scales);
            LogName(how_many_anchors_per_grid);
            LogName(max_grid_height);
            LogName(max_grid_width);
        }

        Yolov3Layer(const vector<pair<float, float>> &_anchors, const int &_classes,
                    const int &_how_many_scales = 3,
                    const int &_max_grid_width = 32, const int &_max_grid_height = 32)
                : anchors(_anchors), classes(_classes), how_many_scales(_how_many_scales),
                  max_grid_width(_max_grid_width), max_grid_height(_max_grid_height) {
            assert(how_many_scales >= 1);// 从多少个尺度进行检测
            assert(anchors.size() % how_many_scales == 0);// 没有多余的anchor
            how_many_anchors_per_grid = anchors.size() / how_many_scales; // 推测每个格子有多少anchor
            assert(how_many_anchors_per_grid >= 1);
            assert(max_grid_width >= 1);
            assert(max_grid_height >= 1);
            int gw = max_grid_width, gh = max_grid_height, loop = how_many_scales;
            while (--loop) {
                assert(gw % 2 == 0);
                gw /= 2;
                assert(gh % 2 == 0);
                gh /= 2;
            }
        }

        /**
         * @param objects 存放解析结果
         * @param scaleOut yolov3在一个尺度上的输出
         * @param scaleIndex 尺度索引
         * @param width 输入图像的宽，要满足倍数要求
         * @param height 输入图像的高，要满足倍数要求
         */
        void handleOneScale(list<Object> &objects,
                            const vector<float> &scaleOut, const int &scaleIndex = 0,
                            const int &width = 416, const int &height = 416) {
            assert(scaleIndex < how_many_scales);
            int gw = max_grid_width, gh = max_grid_height, loop = scaleIndex;
            while (loop--) {// !注意这里是索引
                gw /= 2;
                gh /= 2;
            }
            assert(width % gw == 0 && height % gh == 0);
            const int gridNumX = width / gw, gridNumY = height / gh;
            const int offset = 4 + 1 + classes;
            auto len = scaleOut.size();
//            LogName(len);
//            LogName(gridNumX);
//            LogName(gridNumY);
            assert(len == gridNumX * gridNumY * offset * how_many_anchors_per_grid);
            auto begin = scaleOut.cbegin();
            vector<float>::const_iterator start, end, xIter, yIter, wIter, hIter, confIter;
            vector<float>::iterator maxProbIter;
            float xx, yy, ww, hh;
            auto sigmod = [](const float &x) {
                return 1.0 / (1.0 + exp(-x));
            };
            for (int y = 0; y < gridNumY; y++) {
                for (int x = 0; x < gridNumX; x++) {
                    for (int a = 0; a < how_many_anchors_per_grid; a++) {
                        // 拿到(x,y,a)往后的(4 + 1 + classes)个浮点数
//                        LogName(x);
//                        LogName(y);
//                        LogName(a);
                        start = begin + (y * gridNumX + x) * how_many_anchors_per_grid * offset + a * offset;
                        end = start + offset;
                        const auto &anchor = anchors[scaleIndex * how_many_anchors_per_grid + a];
                        xIter = start;
                        yIter = start + 1;
                        wIter = start + 2;
                        hIter = start + 3;
                        confIter = start + 4;
//                        LogName("@");
//                        transform(confIter, end, cp.begin(), sigmod);
                        vector<float> cp(1 + classes, 0);
                        copy(start + 5, end, cp.begin());
                        for (auto i = cp.begin(); i != cp.end(); i++) { *i = sigmod(*i); }
//                        LogName("#@");
                        for (auto i = cp.begin() + 1; i != cp.end(); i++) { *i *= cp.front(); }
//                        LogName("#");
                        maxProbIter = max_element(cp.begin() + 1, cp.end());
                        auto maxProbIndex = distance(cp.begin() + 1, maxProbIter);
                        xx = (sigmod(*xIter) + x) * gw;
                        yy = (sigmod(*yIter) + y) * gh;
                        ww = exp(*hIter) * anchor.first;
                        hh = exp(*wIter) * anchor.second;
//                        LogName(")");
                        objects.emplace_back(
                                Object(xx, yy, ww, hh, *maxProbIter, maxProbIndex));
//                        LogName("*");
                    }
                }
            }
            LogName(end == scaleOut.cend());
        }

    private:
        // 3
        // wxhx((4+1+11)x3)
        int classes;
        int how_many_scales;// 通常是在2/3个尺度下检测
        vector<pair<float, float>> anchors;
        int how_many_anchors_per_grid;// 支持更多anchor数目（yolov3及其变种几乎都使用3）
        int max_grid_width, max_grid_height;// 支持矩形grid（yolov3及其变种几乎都使用32x32）
    };
}
#endif //_YOLOV3_H
