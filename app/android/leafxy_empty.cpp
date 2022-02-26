#include <QWidget>

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    (new QWidget)->show();
    return a.exec();
}
