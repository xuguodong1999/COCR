QT += quick gui widgets

CONFIG += c++14 resources_big androidextras

DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH +=  $${PWD}/openbabel/include/openbabel3 \
                # FIXME: point to opencv include directory here
                $${PWD}/../lib/opencv/include/opencv4 \
                $${PWD}/../include


LIBS += $${PWD}/openbabel/lib/libopenbabel.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_dnn.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_imgproc.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_calib3d.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_features2d.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_highgui.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_ml.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_stitching.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_core.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_flann.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_imgcodecs.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_objdetect.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_video.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_gapi.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_photo.a \
        $${PWD}/opencv/sdk/native/staticlibs/armeabi-v7a/libopencv_videoio.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/libade.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/libIlmImf.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/liblibjasper.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/liblibpng.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/liblibtiff.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/libquirc.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/libtegra_hal.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/libcpufeatures.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/libittnotify.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/liblibjpeg-turbo.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/liblibprotobuf.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/liblibwebp.a \
        $${PWD}/opencv/sdk/native/3rdparty/libs/armeabi-v7a/libtbb.a

HEADERS += \
    helloworld/glwindow.h \
    helloworld/logo.h

SOURCES += \
    ../src/yolov3.cpp \
    helloworld/glwindow.cpp \
    helloworld/logo.cpp \
    helloworld/main.cpp

RESOURCES += \
    ../res/img.qrc \
    ../res/obabel.qrc \
    ../res/weights.qrc \
    helloworld/hellogles3.qrc

TRANSLATIONS += \
    COCR-android_zh_CN.ts

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    android/AndroidManifest.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew \
    android/gradlew.bat \
    android/res/values/libs.xml

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

