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
