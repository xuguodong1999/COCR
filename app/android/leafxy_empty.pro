QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# disables all the APIs deprecated before Qt 6.0.0

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    

SOURCES += leafxy_empty.cpp


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/settings

DISTFILES += \
    $${ANDROID_PACKAGE_SOURCE_DIR}/AndroidManifest.xml \
    $${ANDROID_PACKAGE_SOURCE_DIR}/build.gradle \
    $${ANDROID_PACKAGE_SOURCE_DIR}/gradle.properties \
    $${ANDROID_PACKAGE_SOURCE_DIR}/gradle/wrapper/gradle-wrapper.jar \
    $${ANDROID_PACKAGE_SOURCE_DIR}/gradle/wrapper/gradle-wrapper.properties \
    $${ANDROID_PACKAGE_SOURCE_DIR}/gradlew \
    $${ANDROID_PACKAGE_SOURCE_DIR}/gradlew.bat \
    $${ANDROID_PACKAGE_SOURCE_DIR}/res/values/libs.xml

