#include "couch.h"
#include <QApplication>
#include <QTextCodec>
#include <QWidget>
#include "isomer.h"

using namespace std;

int main(int argc, char **argv) {
//    cocr::UnrootedTree ut;
//    ut.add(1, 0);
//    ut.add(1, 1);
//    ut.add(1, 2);
//    ut.add(2, 1);
//    cout << ut << endl;
//    auto f = [](const cocr::Graph<unsigned short>& g,const unsigned short& current,const unsigned short& from) {
//        cout << current << " is visited! My father node is "<<from<<" and here are my neighbors: [";
//        for (auto &neighbor:g.data[current]) {
//            cout << neighbor << ",";
//        }
//        cout << "]" << endl;
//
//    };
//    ut.dfsWrapper(f);
////    cout<<sizeof(unsigned)<<endl;
//    return 0;

    cocr::dumpCouchToFile("/home/xgd/dataset/SCUT_IRAC/Couch", "../../backup/dls");
    vector<cocr::SampleData> samples;
    cocr::loadCouchFromFile("../../backup/dls", samples);
    cout << "get " << samples.size() << " samples in main" << endl;
    set<string> labels;
    for (auto &s : samples) {
        labels.insert(s.getLabel());
    }
    cout<<"count="<<labels.size()<<endl;
    for (auto &sample:samples) {
        cout << sample << endl;
        sample.visualize(2);
//        sample.visualize(2, "/tmp/sample.jpg");
    }
    return 0;

    //qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    //QApplication a(argc, argv);
    //(new QWidget)->show();
    //return a.exec();
}