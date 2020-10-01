#ifndef AXXLAB_CVUTILS_H
#define AXXLAB_CVUTILS_H
#define SIGMOID(x) (1.0 / (1.0 + std::exp(-x)))

#include <jni.h>
#include <string>
#include <exception>
#include <opencv2/core/types.hpp>

class RectObj {
public:
    RectObj() {}

    cv::Rect2d boundingBox;
    int idx;
    float conf;
};

extern "C" JNIEXPORT jstring JNICALL
Java_com_xgd_axxlab_detector_DetectorTFLiteImpl_passArrToJNI(
        JNIEnv *env, jobject, jobjectArray objArray,
        jint headNum, jint anchorNum, jint classNum, jint locParmNum,
        jint inputWidth, jint inputHeight, jint downRatio);
#endif //AXXLAB_CVUTILS_H
