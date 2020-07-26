QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $${PWD}/../include

SOURCES += \
    ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/sketchscene.cpp \
    ../src/sketchview.cpp \
    ../src/sketchwidget.cpp \
    ../src/switchbutton.cpp

HEADERS += \
    ../src/mainwindow.h \
    ../src/sketchscene.h \
    ../src/sketchview.h \
    ../src/sketchwidget.h \
    ../src/switchbutton.h

FORMS += \
    ../src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
