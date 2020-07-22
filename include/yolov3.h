#ifndef _YOLOV3_H
#define _YOLOV3_H

#include "cocr.h"
#include <QRectF>
#include <opencv2/dnn.hpp>

namespace cocr {
    class RectObj {
    private:
        float x, y, w, h, conf;
        int index;
    public:
        RectObj() {}

        RectObj(float x, float y, float w, float h, float conf, int index)
                : x(x), y(y), w(w), h(h), conf(conf), index(index) {}

        QRectF toQRectF() {
            return QRectF(x - w / 2, y - h / 2, w, h);
        }

        friend std::ostream &operator<<(std::ostream &out, const RectObj &o) {
            out << "\n[" << o.x << "," << o.y << "," << o.w << "," << o.h << "," << o.conf << "," << o.index << "]";
            return out;
        }

        int getIndex() const {
            return index;
        }

        void setIndex(int index) {
            RectObj::index = index;
        }

        float getX() const {
            return x;
        }

        void setX(float x) {
            RectObj::x = x;
        }

        float getY() const {
            return y;
        }

        void setY(float y) {
            RectObj::y = y;
        }

        float getW() const {
            return w;
        }

        void setW(float w) {
            RectObj::w = w;
        }

        float getH() const {
            return h;
        }

        void setH(float h) {
            RectObj::h = h;
        }

        float getConf() const {
            return conf;
        }

        void setConf(float conf) {
            RectObj::conf = conf;
        }

        float iou(const RectObj &o) const {
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

        friend inline void nms(std::list<RectObj> &objects,
                               const float &conf_thresh = 0.5, const float &iou_thresh = 0.5) {
            for (auto it = objects.begin(); it != objects.end();) {
                if (it->conf < conf_thresh) {
                    it = objects.erase(it);
                } else {
                    it++;
                }
            }
            objects.sort([](const RectObj &a, const RectObj &b) {
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
        void log();

        Yolov3Layer(const std::vector<std::pair<float, float>> &_anchors, const int &_classes,
                    const int &_how_many_scales = 3,
                    const int &_max_grid_width = 32, const int &_max_grid_height = 32);

        /**
         * @param objects 存放解析结果
         * @param scaleOut yolov3在一个尺度上的输出
         * @param scaleIndex 尺度索引
         * @param width 输入图像的宽，要满足倍数要求
         * @param height 输入图像的高，要满足倍数要求
         */
        void handleOneScale(std::list<RectObj> &objects,
                            const std::vector<float> &scaleOut, const int &scaleIndex = 0,
                            const int &width = 416, const int &height = 416);

    private:
        // 3
        // wxhx((4+1+11)x3)
        int classes;
        int how_many_scales;// 通常是在2/3个尺度下检测
        std::vector<std::pair<float, float>> anchors;
        int how_many_anchors_per_grid;// 支持更多anchor数目（yolov3及其变种几乎都使用3）
        int max_grid_width, max_grid_height;// 支持矩形grid（yolov3及其变种几乎都使用32x32）
    };

    cv::dnn::Net getCocrYoloCvImpl(
            const char *cfgPath = ":/weights/enetb0.cfg",
            const char *weightsPath = ":/weights/enetb0.weights"
    );

    ErrorCode forwardCocrCvImpl(const cv::Mat &img, std::vector<cv::Mat> &result);
}
#endif //_YOLOV3_H
