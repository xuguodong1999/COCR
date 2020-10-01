#include "cvutils.h"
#include "axx_ndk.h"

#include <opencv2/opencv.hpp>

#include <jni.h>
#include <string>
#include <exception>

using namespace cv;
extern "C" JNIEXPORT jstring JNICALL
Java_com_xgd_axxlab_detector_DetectorTFLiteImpl_passArrToJNI(
        JNIEnv *env, jobject, jobjectArray objArray,
        jint headNum, jint anchorNum, jint classNum, jint locParmNum,
        jint inputWidth, jint inputHeight, jint downRatio) {
    std::string hello = "Hello from C++\n";
    const float confThresh = 0.3, iouThresh = 0.5;
    const std::vector<std::vector<float>> anchors = {
            {10.,  10.},
            {5.,   30.},
            {30.,  5.},
            {30.,  30.},
            {15.,  90.},
            {90.,  15.},
            {60.,  60.},
            {30.,  180.},
            {180., 30.}
    };
    int gridNumX = inputWidth / downRatio, gridNumY = inputHeight / downRatio;
    int datLength = locParmNum + classNum;
    std::set<int> iSet;
    std::vector<float> confidences;
    std::vector<cv::Rect2d> boxes;
    std::vector<int> ids;
    for (int i = 0; i < headNum; i++) {
        jobject dim4 = env->GetObjectArrayElement(objArray, i);
        jobject dim3 = env->GetObjectArrayElement(
                static_cast<jobjectArray>(dim4), 0);
        for (int x = 0; x < gridNumX; x++) {
            jobject dim2 = env->GetObjectArrayElement(
                    static_cast<jobjectArray>(dim3), x);
            for (int y = 0; y < gridNumY; y++) {
                jobject dim1 = env->GetObjectArrayElement(
                        static_cast<jobjectArray>(dim2), y);
                jboolean isCopied;
                jfloat *dim0 = env->GetFloatArrayElements(
                        static_cast<jfloatArray>(dim1), &isCopied);
//                LOGD("isCopied=%d", isCopied);
                int idx = 0;
                for (int j = 0; j < anchorNum; j++) {
                    int confIdx = idx + 4;
                    float conf = SIGMOID(dim0[confIdx]);
                    if (conf > confThresh) {
                        int maxIdx = confIdx + 1;
                        for (int k = confIdx + 2; k <= confIdx + classNum; k++) {
                            if (dim0[k] > dim0[maxIdx]) {
                                maxIdx = k;
                            }
                        }
                        float finalConf = conf * SIGMOID(dim0[maxIdx]);
                        if (finalConf > confThresh) {
                            float xx, yy, ww, hh;
                            xx = SIGMOID(dim0[idx]) + x;
                            yy = SIGMOID(dim0[idx + 1]) + y;
                            xx *= (inputWidth / gridNumX);
                            yy *= (inputHeight / gridNumY);
                            ww = std::exp(dim0[idx + 2]);
                            hh = std::exp(dim0[idx + 3]);
                            ww *= anchors[(i * 3 + j)][0] / inputWidth;
                            hh *= anchors[(i * 3 + j)][1] / inputHeight;
                            xx -= ww / 2;
                            yy -= hh / 2;
                            boxes.push_back(cv::Rect2d(xx, yy, ww, hh));
                            ids.push_back(maxIdx - confIdx - 1);
                            confidences.push_back(finalConf);
                            // FIXME: delete test code
                            iSet.insert(maxIdx - confIdx - 1);
                        }
                    }
                    idx += datLength;
                }
                if (isCopied) {
                    delete dim0;
                }
            }
        }
        gridNumX *= 2;
        gridNumY *= 2;
    }
    // FIXME: delete test code
    for (auto &i:iSet) LOGD("%d", i);

    std::vector<int> indices;
    cv::dnn::NMSBoxes(boxes, confidences, confThresh, iouThresh, indices);
    std::vector<RectObj> objs;
    objs.resize(indices.size());
    LOGD("size=%d", indices.size());
    for (size_t i = 0; i < indices.size(); i++) {
        objs[i].boundingBox = boxes[indices[i]];
        objs[i].idx = ids[indices[i]];
        objs[i].conf = confidences[indices[i]];
        LOGD("@[%d],%f,%f,%f,%f,*[%d]*,%f",
             indices[i],
             objs[i].boundingBox.tl().x, objs[i].boundingBox.tl().y,
             objs[i].boundingBox.br().x, objs[i].boundingBox.br().y,
             objs[i].idx, objs[i].conf);
    }
    return env->NewStringUTF(hello.c_str());
}


