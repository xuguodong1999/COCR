#ifndef COCR_H
#define COCR_H

#include <QApplication>
#include <QTranslator>
#include <QDesktopWidget>
#include <QDebug>
#include <QSize>
#include <cassert>

//#define MAKE_DATASET

#define look(a)            std::cout<<"exp("<<#a<<")= "<<(a)<<std::endl
#define angle2rad(a)    a * acos(-1) / 180.0
#define rad2angle(r)    r * 180 / acos(-1)

#ifndef COCRAPI_EXPORTS
#define COCRAPI_EXPORTS 1
#endif

#ifndef COCR_EXPORTS
# if (defined _WIN32 || defined WINCE || defined __CYGWIN__) && defined(COCRAPI_EXPORTS)
#   define COCR_EXPORTS __declspec(dllexport)
# elif defined __GNUC__ && __GNUC__ >= 4 && (defined(COCRAPI_EXPORTS) || defined(__APPLE__))
#   define COCR_EXPORTS __attribute__ ((visibility ("default")))
# endif
#endif

#ifndef COCR_EXPORTS
# define COCR_EXPORTS
#endif


const double StdDoubleBondAngle = 2;    //双键和中轴的夹角
const double StdTripleBondAngle = 5;    //三键和中轴的夹角
const double StdWedgeBondAngle = 5;        //楔形键和中轴的夹角
const double StdAtomTextAvgWidth = 20;    //原子文本的大小
const double StdDashBondDense = 6;        //虚楔形键的像素密度
const double StdBestDetectWidth = 40;    //最佳检测尺度

QString getAppDirPath();

void setAppAttribute();

void setAppTranslator(const QString &language);

void moveToCenter(QWidget &_widget, double _w_scale = 0.5, double _h_scale = 0.5);

class QLayout;

void closeLayout(QLayout *l);

class QScrollArea;

class QImage;

QScrollArea *getImageViewer(QImage *image, QSize size);

bool getAndroidPermission();

#endif // COCR_H
