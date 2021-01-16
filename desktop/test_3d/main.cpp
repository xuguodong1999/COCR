#include <QApplication>
#include <QWidget>
#include <QSharedMemory>
int main(int argc,char**args){
    QApplication a(argc,args);
    (new QWidget)->show();
    auto qsm=new QSharedMemory("hello.jpg");
    qsm->create(4,QSharedMemory::ReadWrite);
    *((int*)qsm->data())=5;
//    qsm->detach();
    qsm->attach();
    return a.exec();
}