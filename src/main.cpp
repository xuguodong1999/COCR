#include "cocr.h"
#include "mainwindow.h"

#include <QApplication>
#include "chemdata.h"
#include "tmp.h"
using namespace std;

inline void seeEmptyClassSize();

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
//    QImage img;
//    img.load("D:/0.png");
//    for(int y = 0; y < img.height(); y++){
//        for(int x = 0; x < img.width(); x++){
//            if(img.pixel(x,y)==qRgb(255,255,255)){
//                img.setPixelColor(x,y,QColor(0,0,0,0));
//            }
//        }
//    }
//    img.save("D:/fuck.png");
//    auto &cd = cocr::ChemistryData::GetInstance();
//    for (auto &entry:cd.updateValence()) {
//        for (auto &val:entry.second) {
//            cout << val << endl;
//        }
//    }
    (new MainWindow)->show();
    LogLine("enter event loop...");
    return a.exec();
}
