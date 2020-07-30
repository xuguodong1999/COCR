QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++14

DEFINES += QT_DEPRECATED_WARNINGS

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += $${PWD}/../include

SOURCES += \
    ../src/chemdata.cpp \
    ../src/ioutil.cpp \
    ../src/main.cpp \
    ../src/mainwindow.cpp \
    ../src/sketchscene.cpp \
    ../src/sketchview.cpp \
    ../src/sketchwidget.cpp \
    ../src/switchbutton.cpp

HEADERS += \
    ../include/chemdata.h \
    ../include/cocr.h \
    ../include/couch.h \
    ../include/equation.h \
    ../include/fraction.h \
    ../include/graph.h \
    ../include/ioutil.h \
    ../include/isomer.h \
    ../include/solver.h \
    ../include/tmp.h \
    ../src/mainwindow.h \
    ../src/sketchscene.h \
    ../src/sketchview.h \
    ../src/sketchwidget.h \
    ../src/switchbutton.h

msvc {
    RC_ICONS = $${PWD}\..\\res\\img\\jellyxu.ico
    QMAKE_TARGET_COMPANY=1
    QMAKE_TARGET_DESCRIPTION=2
    QMAKE_TARGET_COPYRIGHT=3
    QMAKE_TARGET_PRODUCT=4
    RC_CODEPAGE=5
    RC_LANG=6
    VERSION=1.0
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8 /openmp
}

FORMS += \
    ../src/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
