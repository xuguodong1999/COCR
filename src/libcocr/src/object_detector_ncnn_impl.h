#pragma once

#include "cocr/object_detector.h"

#include <ncnn/net.h> // <ncnn/net.h>
#include <ncnn/datareader.h> // <ncnn/datareader.h>

#include <opencv2/core/types.hpp> // cv::Rect

#include <QFile>
#include <QDebug>

#include <string>
#include <memory>
#include <vector>

#define YOLOX_NMS_THRESH  0.45 // nms threshold
#define YOLOX_CONF_THRESH 0.25 // threshold of bounding box prob

// YOLOX use the same focus in yolov5
class YoloV5Focus : public ncnn::Layer {
public:
    YoloV5Focus() {
        one_blob_only = true;
    }

    virtual int forward(const ncnn::Mat &bottom_blob, ncnn::Mat &top_blob, const ncnn::Option &opt) const {
        int w = bottom_blob.w;
        int h = bottom_blob.h;
        int channels = bottom_blob.c;

        int outw = w / 2;
        int outh = h / 2;
        int outc = channels * 4;

        top_blob.create(outw, outh, outc, 4u, 1, opt.blob_allocator);
        if (top_blob.empty())
            return -100;

#pragma omp parallel for num_threads(opt.num_threads)
        for (int p = 0; p < outc; p++) {
            const float *ptr = bottom_blob.channel(p % channels).row((p / channels) % 2) + ((p / channels) / 2);
            float *outptr = top_blob.channel(p);

            for (int i = 0; i < outh; i++) {
                for (int j = 0; j < outw; j++) {
                    *outptr = *ptr;

                    outptr += 1;
                    ptr += 2;
                }

                ptr += w;
            }
        }

        return 0;
    }
};

DEFINE_LAYER_CREATOR(YoloV5Focus)

DEFINE_LAYER_DESTROYER(YoloV5Focus)

struct Object {
    cv::Rect_<float> rect;
    int label;
    float prob;
};

struct GridAndStride {
    int grid0;
    int grid1;
    int stride;
};

static inline float intersection_area(const Object &a, const Object &b) {
    cv::Rect_<float> inter = a.rect & b.rect;
    return inter.area();
}

static void qsort_descent_inplace(std::vector<Object> &faceobjects, int left, int right) {
    int i = left;
    int j = right;
    float p = faceobjects[(left + right) / 2].prob;

    while (i <= j) {
        while (faceobjects[i].prob > p)
            i++;

        while (faceobjects[j].prob < p)
            j--;

        if (i <= j) {
            // swap
            std::swap(faceobjects[i], faceobjects[j]);

            i++;
            j--;
        }
    }

#pragma omp parallel sections
    {
#pragma omp section
        {
            if (left < j) qsort_descent_inplace(faceobjects, left, j);
        }
#pragma omp section
        {
            if (i < right) qsort_descent_inplace(faceobjects, i, right);
        }
    }
}

static void qsort_descent_inplace(std::vector<Object> &objects) {
    if (objects.empty())
        return;

    qsort_descent_inplace(objects, 0, objects.size() - 1);
}

static void nms_sorted_bboxes(const std::vector<Object> &faceobjects, std::vector<int> &picked, float nms_threshold) {
    picked.clear();

    const int n = faceobjects.size();

    std::vector<float> areas(n);
    for (int i = 0; i < n; i++) {
        areas[i] = faceobjects[i].rect.area();
    }

    for (int i = 0; i < n; i++) {
        const Object &a = faceobjects[i];

        int keep = 1;
        for (int j = 0; j < (int) picked.size(); j++) {
            const Object &b = faceobjects[picked[j]];

            // intersection over union
            float inter_area = intersection_area(a, b);
            float union_area = areas[i] + areas[picked[j]] - inter_area;
            // float IoU = inter_area / union_area
            if (inter_area / union_area > nms_threshold)
                keep = 0;
        }

        if (keep)
            picked.push_back(i);
    }
}

static void
generate_grids_and_stride(const int &w, const int &h, std::vector<int> &strides,
                          std::vector<GridAndStride> &grid_strides) {
    for (int i = 0; i < (int) strides.size(); i++) {
        int stride = strides[i];
        int num_grid_w = w / stride;
        int num_grid_h = h / stride;
        for (int g1 = 0; g1 < num_grid_h; g1++) {
            for (int g0 = 0; g0 < num_grid_w; g0++) {
                GridAndStride gs;
                gs.grid0 = g0;
                gs.grid1 = g1;
                gs.stride = stride;
                grid_strides.push_back(gs);
            }
        }
    }
}

static void
generate_yolox_proposals(std::vector<GridAndStride> grid_strides, const ncnn::Mat &feat_blob, float prob_threshold,
                         std::vector<Object> &objects) {
    const int num_grid = feat_blob.h;
    const int num_class = feat_blob.w - 5;
    const int num_anchors = grid_strides.size();

    const float *feat_ptr = feat_blob.channel(0);
    for (int anchor_idx = 0; anchor_idx < num_anchors; anchor_idx++) {
        const int grid0 = grid_strides[anchor_idx].grid0;
        const int grid1 = grid_strides[anchor_idx].grid1;
        const int stride = grid_strides[anchor_idx].stride;

        // yolox/models/yolo_head.py decode logic
        //  outputs[..., :2] = (outputs[..., :2] + grids) * strides
        //  outputs[..., 2:4] = torch.exp(outputs[..., 2:4]) * strides
        float x_center = (feat_ptr[0] + grid0) * stride;
        float y_center = (feat_ptr[1] + grid1) * stride;
        float w = exp(feat_ptr[2]) * stride;
        float h = exp(feat_ptr[3]) * stride;
        float x0 = x_center - w * 0.5f;
        float y0 = y_center - h * 0.5f;

        float box_objectness = feat_ptr[4];
        for (int class_idx = 0; class_idx < num_class; class_idx++) {
            float box_cls_score = feat_ptr[5 + class_idx];
            float box_prob = box_objectness * box_cls_score;
            if (box_prob > prob_threshold) {
                Object obj;
                obj.rect.x = x0;
                obj.rect.y = y0;
                obj.rect.width = w;
                obj.rect.height = h;
                obj.label = class_idx;
                obj.prob = box_prob;

                objects.push_back(obj);
            }

        } // class loop
        feat_ptr += feat_blob.w;

    } // point anchor loop
}

class ObjectDetectorNcnnImpl : public ObjectDetector {
    int numThread;

    std::shared_ptr<ncnn::Net> net;

public:
    void setNumThread(int numThread) {
        ObjectDetectorNcnnImpl::numThread = numThread;
        if (net) { net->opt.num_threads = numThread; }
    }

    ObjectDetectorNcnnImpl() : numThread(4), net(nullptr) {

    }

    bool initModel(const std::string &_ncnnBin, const std::string &_ncnnParam, const int &_maxWidth) {
        maxWidth = maxHeight = _maxWidth - sizeBase;
        QFile cfgFile(_ncnnParam.c_str()), weightsFile(_ncnnBin.c_str());
        if (!cfgFile.open(QIODevice::ReadOnly) || !weightsFile.open(QIODevice::ReadOnly)) {
            qDebug() << "fail in QFile read" << _ncnnBin.c_str() << "and" << _ncnnParam.c_str();
            return false;
        }
        QByteArray cfg = cfgFile.readAll();
        cfgFile.close();
        QByteArray weights = weightsFile.readAll();
        weightsFile.close();

        try {
            net = std::make_shared<ncnn::Net>();
            net->opt.num_threads = numThread;
#if defined(__ANDROID__) && __ANDROID_API__ >= 26
            net->opt.use_vulkan_compute = true;
#endif
            net->opt.use_winograd_convolution = true;
            net->opt.use_sgemm_convolution = true;
            net->opt.use_fp16_packed = true;
            net->opt.use_fp16_storage = true;
            net->opt.use_fp16_arithmetic = true;
            net->opt.use_packing_layout = true;
            net->opt.use_shader_pack8 = false;
            net->opt.use_image_storage = false;
            net->register_custom_layer("YoloV5Focus", YoloV5Focus_layer_creator);
            const unsigned char *cfgMem = (const unsigned char *) cfg.data();
            ncnn::DataReaderFromMemory cfgReader(cfgMem);
            int ret_param = net->load_param(cfgReader);
            if (ret_param != 0) {
                qDebug() << "net->load_param(cfgReader) dies";
                return false;
            }
            const unsigned char *weightsMem = (const unsigned char *) weights.data();
            ncnn::DataReaderFromMemory weightsReader(weightsMem);
            int ret_bin = net->load_model(weightsReader);
            if (ret_bin != 0) {
                qDebug() << "net->load_model(weightsReader) dies";
                return false;
            }

            std::vector<Mat> outs;
            Mat emptyBlob(MatChannel::GRAY, DataType::UINT8, 32, 32);
            ncnn::Mat in = ncnn::Mat::from_pixels(
                    emptyBlob.getData(), ncnn::Mat::PIXEL_GRAY,
                    emptyBlob.getWidth(), emptyBlob.getHeight());
            ncnn::Extractor ex = net->create_extractor();
            ex.input("images", in);
            ncnn::Mat out;
            ex.extract("output", out);
        } catch (std::exception &e) {
            qDebug() << __FUNCTION__ << "catch" << e.what();
            return false;
        }
        return true;
    }

    void freeModel() override {
        net->clear();
        net = nullptr;
    }


    std::pair<Mat, std::vector<DetectorObject>>
    detect(const Mat &_originImage) override {
        Mat input = preProcess(_originImage);
        int img_w = input.getWidth();
        int img_h = input.getHeight();
        ncnn::Mat in = ncnn::Mat::from_pixels(
                input.getData(), ncnn::Mat::PIXEL_GRAY,
                img_w, img_h);

        ncnn::Extractor ex = net->create_extractor();

        ex.input("images", in);

        std::vector<Object> proposals;

        {
            ncnn::Mat out;
            ex.extract("output", out);

            static const int stride_arr[] = {8, 16, 32}; // might have stride=64 in YOLOX
            std::vector<int> strides(stride_arr, stride_arr + sizeof(stride_arr) / sizeof(stride_arr[0]));
            std::vector<GridAndStride> grid_strides;
            generate_grids_and_stride(img_w, img_h, strides, grid_strides);
            generate_yolox_proposals(grid_strides, out, YOLOX_CONF_THRESH, proposals);
        }

        // sort all proposals by score from highest to lowest
        qsort_descent_inplace(proposals);

        // apply nms with nms_threshold
        std::vector<int> picked;
        nms_sorted_bboxes(proposals, picked, YOLOX_NMS_THRESH);

        int count = picked.size();
        std::vector<DetectorObject> objects;
        float scale = 1.f;
        for (int i = 0; i < count; i++) {
            const auto &obj = proposals[picked[i]];

            // adjust offset to original unpadded
            float x0 = (obj.rect.x) / scale;
            float y0 = (obj.rect.y) / scale;
            float x1 = (obj.rect.x + obj.rect.width) / scale;
            float y1 = (obj.rect.y + obj.rect.height) / scale;

            // clip
            x0 = std::max(std::min(x0, (float) (img_w - 1)), 0.f);
            y0 = std::max(std::min(y0, (float) (img_h - 1)), 0.f);
            x1 = std::max(std::min(x1, (float) (img_w - 1)), 0.f);
            y1 = std::max(std::min(y1, (float) (img_h - 1)), 0.f);

            if (DetectorObject::isValidLabel(obj.label)) {
                objects.emplace_back(
                        x0,
                        y0,
                        x1 - x0,
                        y1 - y0,
                        obj.label,
                        obj.prob);
            }
        }
        return {input, objects};
    }
};
