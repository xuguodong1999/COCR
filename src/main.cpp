#include "couch.h"
#include <QApplication>
#include <QWidget>
#include <set>
#include <QTextCodec>
using namespace cocr;

int main(int argc, char **argv) {
    vector<SampleData> samples;
    auto &couchReader = CouchReader::GetInstance();

//    // 读原始数据集，序列化成单独文件
//    couchReader.readCouchTopDir("G:/dataset/SCUT_IRAC/Couch", samples);
//    cout << samples.size() << endl;
//    couchReader.washDataSet(samples);
//    cout << samples.size() << endl;
//    QFile f("/tmp/couch.txt");
//    f.open(QIODevice::WriteOnly);
//    if (!f.isOpen()) {
//        return -1;
//    }
//    QDataStream writter(&f);
//    writter << (int) samples.size();
//    for (auto &s:samples) {
//        writter << s;
//    }
//    f.close();
//    ofstream ofsm("/tmp/labels.txt");
//    set <string> labels;
//    for (auto &s : samples) {
//        labels.insert(s.getLabel());
//    }
//    for (auto &l : labels) {
//        ofsm << l << endl;
//    }
//    ofsm.close();

    // 从序列化文件加载数据
#ifdef Q_OS_UNIX
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("gb2312"));
#endif
    QFile f2("../../backup/dls/couch.txt");
    f2.open(QIODevice::ReadOnly);
    QDataStream reader(&f2);
    int size;
    reader>>size;
    samples.resize(size);
    for(auto&s:samples){
        reader>>s;
    }
    f2.close();
    cout<<samples.size()<<endl;
    random_shuffle(samples.begin(), samples.end());
    couchReader.washDataSet(samples);
    cout<<samples.size()<<endl;
    return 0;

    //qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    //QApplication a(argc, argv);
    //(new QWidget)->show();
    //return a.exec();

}