#include <QApplication>
#include <QWidget>

using namespace std;

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    (new QWidget)->show();
    return a.exec();
}