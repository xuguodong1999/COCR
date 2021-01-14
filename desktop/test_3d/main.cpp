#include <QApplication>
#include <QWidget>
int main(int argc,char**args){
    QApplication a(argc,args);
    (new QWidget)->show();
    return a.exec();
}