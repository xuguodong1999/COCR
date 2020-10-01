#ifndef AXXLAB_NDK_HELPER_H
#define AXXLAB_NDK_HELPER_H

#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>
#include <android/log.h>

#include <string>
#include <utility>

#define LOG_TAG "axxx_ndk"
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

#define RET_EMPTY_STRING_IF_NOT(condition) if(!(condition))return ""
#define RET_FALSE_IF_NOT(condition) if(!(condition))return false

/**
 * read file contents from assets directory
 * @param filename
 * @param env
 * @param assetManager
 * @return !!! you need to delete ret value manually
 */
std::pair<char *, size_t> readAssetsFile(
        const char *filename,
        JNIEnv *env, jobject assetManager);

#endif //AXXLAB_NDK_HELPER_H
