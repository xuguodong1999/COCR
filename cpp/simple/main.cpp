//#include "config.hpp"
#include <QImage>
#include <QFont>
#include <QPainter>
#include <QDebug>
#include <QApplication>
#include <QFontDatabase>
#include <opencv2/opencv.hpp>
#include "fontpixitem.hpp"

using namespace std;



extern std::unordered_map<int, std::wstring> gbTable;

int main(int argc, char **argv) {
    qputenv("QML_DISABLE_DISK_CACHE", "1");
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QFontDatabase database;
    auto avalableFontFamilies=database.families();
    avalableFontFamilies.removeOne("ZWAdobeF");
    try {
//        for(auto&ff:avalableFontFamilies){
//            FontPixItem::GetFont("鞫",ff);
//        }
        for (auto&[_, text]:gbTable) {
            FontPixItem::GetFont(
                    QString::fromStdWString(text),
                    "ZWAdobeF"
//                    avalableFontFamilies[rand() % avalableFontFamilies.size()]
            );
        }
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
    }
    exit(-1);
    return app.exec();
}
