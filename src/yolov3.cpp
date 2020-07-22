#include "yolov3.h"
#include <opencv2/opencv.hpp>
#include <cassert>
#include <QByteArray>
#include <QFile>

using namespace std;
using namespace cv;
using namespace dnn;
using namespace chrono;
namespace cocr {
    void Yolov3Layer::log() {
        LogName(classes);
        LogName(how_many_scales);
        LogName(how_many_anchors_per_grid);
        LogName(max_grid_height);
        LogName(max_grid_width);
    }

    Yolov3Layer::Yolov3Layer(const vector<pair<float, float>> &_anchors, const int &_classes,
                             const int &_how_many_scales,
                             const int &_max_grid_width, const int &_max_grid_height)
            : anchors(_anchors), classes(_classes), how_many_scales(_how_many_scales),
              max_grid_width(_max_grid_width), max_grid_height(_max_grid_height) {
        assert(how_many_scales >= 1);                                 // 从多少个尺度进行检测
        assert(anchors.size() % how_many_scales == 0);                // 没有多余的anchor
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
    void Yolov3Layer::handleOneScale(list<RectObj> &objects,
                                     const vector<float> &scaleOut, const int &scaleIndex,
                                     const int &width, const int &height) {
        assert(scaleIndex < how_many_scales);
        int gw = max_grid_width, gh = max_grid_height, loop = scaleIndex;
        while (loop--) { // !注意这里是索引
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
                    for (auto i = cp.begin(); i != cp.end(); i++) {
                        *i = sigmod(*i);
                    }
                    //                        LogName("#@");
                    for (auto i = cp.begin() + 1; i != cp.end(); i++) {
                        *i *= cp.front();
                    }
                    //                        LogName("#");
                    maxProbIter = max_element(cp.begin() + 1, cp.end());
                    auto maxProbIndex = distance(cp.begin() + 1, maxProbIter);
                    xx = (sigmod(*xIter) + x) * gw;
                    yy = (sigmod(*yIter) + y) * gh;
                    ww = exp(*hIter) * anchor.first;
                    hh = exp(*wIter) * anchor.second;
                    //                        LogName(")");
                    objects.emplace_back(
                            RectObj(xx, yy, ww, hh, *maxProbIter, maxProbIndex));
                    //                        LogName("*");
                }
            }
        }
        LogName(end == scaleOut.cend());
    }

    cv::dnn::Net getCocrYoloCvImpl(const char *cfgPath, const char *weightsPath) {
        QByteArray weights, cfg;
        QFile cf(cfgPath);
        cf.open(QIODevice::ReadOnly);
        QFile wf(weightsPath);
        wf.open(QIODevice::ReadOnly);
        cfg = cf.readAll();
        cf.close();
        weights = wf.readAll();
        wf.close();
        auto net = cv::dnn::readNetFromDarknet(cfg.constData(), cfg.length(), weights.constData(), weights.length());
        return net;
    }

    ErrorCode forwardCocrCvImpl(const cv::Mat &img, std::vector<cv::Mat> &result) {
        try {
            static auto net = getCocrYoloCvImpl();
            net.setPreferableBackend(dnn::DNN_BACKEND_OPENCV);
            net.setPreferableTarget(dnn::DNN_TARGET_CPU);
            auto outLayerIndices = net.getUnconnectedOutLayers();
            auto layerNames = net.getLayerNames();
            std::vector<cv::String> outBlobNames(outLayerIndices.size());
            for (auto i = 0; i < outLayerIndices.size(); i++) {
                outBlobNames[i] = layerNames.at(outLayerIndices[i] - 1);
            }
            int w = img.rows - img.rows % 32, h = img.cols - img.cols % 32;
            if (w < 32)
                w = 32;

            if (h < 32)
                h = 32;

            Size inputSize(w, h);
            cv::Mat vec4d;
            blobFromImage(img, vec4d, 1 / 255.0, inputSize);
            net.setInput(vec4d);
            auto start = system_clock::now();
            result.clear();
            net.forward(result, outBlobNames);

            auto duration = duration_cast<microseconds>(system_clock::now() - start);
            qDebug() << "forwardCocrCvImpl takes "
                     << duration.count() * milliseconds::period::num / milliseconds::period::den << "ms";
        }
        catch (std::exception e) {
            return cocr::ErrorCode::CvDnnError;
        }
        return cocr::ErrorCode::NormalExec;
    }
} // namespace cocr
