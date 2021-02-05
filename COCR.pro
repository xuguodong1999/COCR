QT += core 3danimation 3dcore 3dextras 3drender

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17 big_resources# console

DEFINES += QT_DEPRECATED_WARNINGS QT_DISABLE_DEPRECATED_BEFORE=0x060000

msvc:{
# for debug build, compile oencv and openbabel from source
#    QMAKE_CFLAGS_DEBUG += -MTd
#    QMAKE_CXXFLAGS_DEBUG += -MTd

    QMAKE_CFLAGS_RELEASE += -MT
    QMAKE_CXXFLAGS_RELEASE += -MT

    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8 /openmp

    RC_FILE = $${PWD}/src/resources/COCR.rc

    MY_MSVC_LIB_DIR = $${PWD}/msvc_x64_lib

    INCLUDEPATH += $${MY_MSVC_LIB_DIR}/include

    LIB_NAME_LIST = openbabel-3.lib \
#        opencv_highgui451.lib \
        opencv_imgcodecs451.lib opencv_imgproc451.lib \
        opencv_dnn451.lib opencv_core451.lib \
        ade.lib libopenjp2.lib libwebp.lib IlmImf.lib \
        libpng.lib  ittnotify.lib zlib.lib \
        libprotobuf.lib libjpeg-turbo.lib libtiff.lib

    for(LIB_NAME, LIB_NAME_LIST){
        LIBS += $${MY_MSVC_LIB_DIR}/x64/$${LIB_NAME}
    }
}

unix:!android{
    # TODO: set your opencv dir here.
    # >=4.5.0 is required, 4.4.0 is not ok.
    MY_OPENCV_DIR=/home/xgd/shared/opencv4.5.1
    INCLUDEPATH += /usr/include/openbabel3 \
        $${MY_OPENCV_DIR}/include/opencv4
    LIBS += -L$${MY_OPENCV_DIR}/lib -lopenbabel \
        -lopencv_imgcodecs -lopencv_imgproc -lopencv_dnn \
        -lopencv_core -lopencv_highgui
}

android:{
    # extract 4.5.1 for OpenCV-android-sdk
    OPENCV_SDK_DIR = $${PWD}/android/OpenCV-android-sdk

    INCLUDEPATH += $${OPENCV_SDK_DIR}/sdk/native/jni/include

    OPENCV_STATICLIB_DIR = $${OPENCV_SDK_DIR}/sdk/native/staticlibs

    OPENCV_3RDPARTY_DIR = $${OPENCV_SDK_DIR}/sdk/native/3rdparty/libs

    OPENCV_NATIVE_LIST = libopencv_dnn.a libopencv_highgui.a \
        libopencv_imgcodecs.a libopencv_imgproc.a libopencv_core.a

    for(LIB_NAME, OPENCV_NATIVE_LIST){
        LIBS += \
            $${OPENCV_STATICLIB_DIR}/$${ANDROID_TARGET_ARCH}/$${LIB_NAME}
    }

    OPENCV_3RDPARTY_LIST = libquirc.a \
        libade.a libIlmImf.a liblibopenjp2.a liblibjpeg-turbo.a \
        liblibpng.a liblibprotobuf.a liblibtiff.a liblibwebp.a \
        libcpufeatures.a libittnotify.a libtbb.a

    equals(ANDROID_TARGET_ARCH, x86){
        OPENCV_3RDPARTY_LIST += libippiw.a libippicv.a
    }
    equals(ANDROID_TARGET_ARCH, x86_64){
        OPENCV_3RDPARTY_LIST += libippiw.a libippicv.a
    }
    equals(ANDROID_TARGET_ARCH, armeabi-v7a){
        OPENCV_3RDPARTY_LIST += libtegra_hal.a
    }
    equals(ANDROID_TARGET_ARCH, arm64-v8a) {
        OPENCV_3RDPARTY_LIST += libtegra_hal.a
    }
    for(LIB_NAME, OPENCV_3RDPARTY_LIST){
        LIBS += $${OPENCV_3RDPARTY_DIR}/$${ANDROID_TARGET_ARCH}/$${LIB_NAME}
    }
}

INCLUDE_LIST = alkane_graph.hpp hw_script.hpp mol3dwindow.hpp \
    qt_sketchwidget.hpp  atom.hpp hw_stroke.hpp openbabel_util.hpp \
    soso17_converter.hpp bond.hpp mol.hpp opencv_util.hpp std_util.hpp \
    bond_type.hpp mol_holder.hpp yolo.hpp cocr_types.hpp mol_op.hpp \
    qt_mainwindow.hpp yolo_object.hpp colors.hpp mol_util.hpp \
    qt_mol3deditor.hpp yolo_opencv.hpp fraction.hpp mol3d.hpp \
    qt_molfileeditor.hpp hw_base.hpp mol3d_builder.hpp \
    qt_ocrthread.hpp qt_util.hpp

SRC_LIST = atom.cpp main.cpp mol3dwindow.cpp qt_ocrthread.cpp \
    bond.cpp mol.cpp openbabel_util.cpp qt_sketchwidget.cpp \
    cocr_types.cpp mol_holder.cpp opencv_util.cpp \
    soso17_converter.cpp colors.cpp mol_op.cpp std_util.cpp \
    hw_base.cpp mol_util.cpp qt_mainwindow.cpp yolo.cpp \
    hw_script.cpp mol3d.cpp qt_mol3deditor.cpp \
    yolo_opencv.cpp hw_stroke.cpp mol3d_builder.cpp \
    qt_molfileeditor.cpp qt_util.cpp

UI_LIST = qt_mainwindow.ui qt_mol3deditor.ui qt_molfileeditor.ui

for(INCLUDE_FILE,INCLUDE_LIST){
    HEADERS += $${PWD}/src/$${INCLUDE_FILE}
}
for(SRC_FILE,SRC_LIST){
    SOURCES += $${PWD}/src/$${SRC_FILE}
}
for(UI_FILE,UI_LIST){
    FORMS += $${PWD}/src/$${UI_FILE}
}

RESOURCES += $${PWD}/src/resources/res.qrc \
    $${PWD}/src/resources/big_res.qrc

TRANSLATIONS += $${PWD}/src/resources/COCR_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
