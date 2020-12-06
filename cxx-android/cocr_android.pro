QT += core gui quick 3danimation 3dcore 3dextras 3dinput 3dlogic 3drender

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 

# disables all the APIs deprecated before Qt 6.0.0
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    

msvc {
    RC_FILE = demo.rc
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8 /openmp
}

# LIB_PREFIX = J:/
LIB_PREFIX = $${PWD}/ndk-lib
SRC_PREFIX = $${PWD}/../cxx
OPENCV_PREFIX = $${LIB_PREFIX}/opencv4.5.0
OPENBABEL_PREFIX = $${LIB_PREFIX}/openbabel3.1.1

INCLUDEPATH += $${LIB_PREFIX}/include

ANDROID_ABIS = armeabi-v7a
    
LIBS += $${OPENBABEL_PREFIX}/armeabi-v7a/libopenbabel.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_dnn.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_imgproc.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_calib3d.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_features2d.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_highgui.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_ml.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_stitching.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_core.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_flann.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_imgcodecs.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_objdetect.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_video.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_gapi.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_photo.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libopencv_videoio.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libade.a \
        $${OPENCV_PREFIX}/armeabi-v7a/liblibopenjp2.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libquirc.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libcpufeatures.a \
        $${OPENCV_PREFIX}/armeabi-v7a/liblibpng.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libtbb.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libIlmImf.a \
        $${OPENCV_PREFIX}/armeabi-v7a/liblibprotobuf.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libtegra_hal.a \
        $${OPENCV_PREFIX}/armeabi-v7a/libittnotify.a \
        $${OPENCV_PREFIX}/armeabi-v7a/liblibtiff.a \
        $${OPENCV_PREFIX}/armeabi-v7a/liblibjpeg-turbo.a \
        $${OPENCV_PREFIX}/armeabi-v7a/liblibwebp.a


INCLUDEPATH += $${SRC_PREFIX}/src

HEADERS += $${SRC_PREFIX}/src/qml_sketchitem.hpp \
        $${SRC_PREFIX}/src/mol3dwindow.hpp \
        $${SRC_PREFIX}/src/mol3d.hpp
        
SOURCES +=  $${SRC_PREFIX}/app/main_3d.cpp \
         $${SRC_PREFIX}/src/box2graph.cpp \
         $${SRC_PREFIX}/src/colors.cpp \
         $${SRC_PREFIX}/src/mol.cpp \
         $${SRC_PREFIX}/src/atom.cpp \
         $${SRC_PREFIX}/src/bond.cpp \
         $${SRC_PREFIX}/src/mol_hw.cpp \
         $${SRC_PREFIX}/src/mol3d.cpp \
         $${SRC_PREFIX}/src/config.cpp \
         $${SRC_PREFIX}/src/std_util.cpp \
         $${SRC_PREFIX}/src/opencv_util.cpp \
         $${SRC_PREFIX}/src/opencv_yolo.cpp \
         $${SRC_PREFIX}/src/qt_util.cpp \
         $${SRC_PREFIX}/src/couch_data.cpp \
         $${SRC_PREFIX}/src/hw.cpp \
         $${SRC_PREFIX}/src/polya.cpp \
         $${SRC_PREFIX}/src/bignumber.cpp \
         $${SRC_PREFIX}/src/isomer.cpp \
         $${SRC_PREFIX}/src/cocr_types.cpp \
         $${SRC_PREFIX}/src/timer.cpp \
         $${SRC_PREFIX}/src/mol3dwindow.cpp \
         $${SRC_PREFIX}/src/qml_sketchitem.cpp

RESOURCES += $${SRC_PREFIX}/app/res/res.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
