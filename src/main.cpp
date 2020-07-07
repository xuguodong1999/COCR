#include "cocr.h"
#include "couch.h"
#include "isomer.h"
#include <QApplication>
#include <QTextCodec>
#include <QWidget>

using namespace std;

void saveCouchToFile() {
    cocr::dumpCouchToFile(
            "/home/xgd/dataset/SCUT_IRAC/Couch",
            "../../backup/gb2"
    );
}

int main(int argc, char **argv) {
//    auto &ic = cocr::IsomerCounter::getInstance();
//    ic.calculate(6);
//    cocr::show(ic.counter);
    typedef unsigned char number;
    stringstream input("6 1 "
                       "5 1 "
                       "2 1 "
                       "3 1 "
                       "7 2 "
                       "0 3 "
                       "0 4 ");
    cocr::UGraph<number> ut;
    input >> ut;
    LogLine(ut);
    LogName(ut.toString());
    return 0;
//     qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
//     QApplication a(argc, argv);
//     (new QWidget)->show();
//     return a.exec();
}

namespace learn {
    class A {//1
    } a;

    class B {//8
    public:
        virtual void hehe() {}
    } b;

    class B1 : public B {//8
    public:
        void hehe() override {}
    } b1;

    class C {
    public:
        virtual void hehe() = 0;
    };

    class C1 : public C {//8
    public:
        void hehe() override {}
    } c1;

    void hello() {
        cout << sizeof(a) << "," << sizeof(b) << "," << sizeof(b1) << "," << sizeof(c1) << "," << endl;
    }
}
