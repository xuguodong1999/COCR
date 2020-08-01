#include "cocr.h"
#include "mainwindow.h"
#include <QFontDatabase>
#include <QApplication>

using namespace std;

inline void seeEmptyClassSize();

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    QFontDatabase::addApplicationFont(":/lang/simhei.subset.ttf");
    qApp->setFont(QFont("simhei"));
    (new MainWindow)->show();
    LogLine("enter event loop...");
    return a.exec();
}

void test() {
    //    QImage img;
//    img.load("D:/0.ico");
//    img=img.convertToFormat(QImage::Format_RGBA64);
//    for(int y = 0; y < img.height(); y++){
//        for(int x = 0; x < img.width(); x++){
//            if(img.pixel(x,y)==qRgb(255,0,0)){
//                img.setPixelColor(x,y,QColor(0,0,0,0));
//            }
//        }
//    }
//    img.scaled(512,512).save("D:/fuck.ico","ico",9);
//    auto &cd = cocr::ChemistryData::GetInstance();
//    for (auto &entry:cd.updateValence()) {
//        for (auto &val:entry.second) {
//            cout << val << endl;
//        }
//    }
}
