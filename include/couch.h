#ifndef _COUCH_H_
#define _COUCH_H_

#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDataStream>

#include <opencv2/opencv.hpp>

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace cocr {
    using namespace std;

    class StrokeData {
    private:
        unsigned short int strokePointNum;
        typedef cv::Point Point;
        vector<Point> points;
    public:

        void transform(const cv::Mat &affineTransform) {
//            vector<cv::Point> src={
//
//            },dst={
//
//            };
//            auto t= cv::getAffineTransform(src,dst);
            vector<cv::Point2f> src = {
                    cv::Point2f(0, 0),
                    cv::Point2f(0, 20),
                    cv::Point2f(10, 10)
            }, dst = {
                    cv::Point2f(5, 0),
                    cv::Point2f(10, 5),
                    cv::Point2f(10, 0)
            };
            auto t = cv::getAffineTransform(src.data(), dst.data());
            cout << t << endl;
            cv::Mat res;
            cv::multiply(t, cv::Vec3f(5, 5, 1).t(), res);
            cout << res << endl;
        }

        decltype(points.size()) getPointSize() const {
            return points.size();
        }

        const decltype(points) &getPoints() const {
            return points;
        }

        void setStrokePointNum(unsigned short strokePointNum) {
            StrokeData::strokePointNum = strokePointNum;
        }

        StrokeData() : strokePointNum(0) {}

        StrokeData(unsigned short int _strokePointNum) :
                strokePointNum(_strokePointNum) {
            points.resize(strokePointNum);
        }

        friend class SampleData;

        friend inline istream &operator>>(istream &in, StrokeData &sd) {
//              in >> sd.strokePointNum;
            in.read((char *) &sd.strokePointNum, 2);
            sd.points.resize(sd.strokePointNum);
            for (auto &point:sd.points) {
//                  in >> point.first >> point.second;
                in.read((char *) &point.x, 2);
                in.read((char *) &point.y, 2);
            }
            return in;
        }

        friend inline ostream &operator<<(ostream &out, const StrokeData &sd) {
            out << "下一个笔画有" << sd.strokePointNum << "个点" << endl;
            for (const auto &point:sd.points) {
                out << "(" << point.x << "," << point.y << "), ";
            }
            out << endl;
            return out;
        }

        friend inline QDataStream &operator<<(QDataStream &s, const StrokeData &o) {
            s << o.strokePointNum;
            for (auto &point:o.points) {
                s << point.x << point.y;
            }
            return s;
        }

        friend inline QDataStream &operator>>(QDataStream &s, StrokeData &o) {
            s >> o.strokePointNum;
            o.points.resize(o.strokePointNum);
            for (auto &point:o.points) {
                s >> point.x >> point.y;
            }
            return s;
        }
    };

    class SampleData {
    private:
        unsigned short int lineNum;
        vector<StrokeData> strokes;
        string label;
        int width, height;
    public:
        int getWidth() const {
            return width;
        }

        void setWidth(int width) {
            SampleData::width = width;
        }

        int getHeight() const {
            return height;
        }

        void setHeight(int height) {
            SampleData::height = height;
        }

        decltype(strokes.size()) getStrokeSize() const {
            return strokes.size();
        }

        const decltype(strokes) &getStrokes() const {
            return strokes;
        }

        const string &getLabel() const {
            return label;
        }

        void setLabel(const string &label) {
            SampleData::label = label;
        }

        void setLineNum(unsigned short lineNum) {
            SampleData::lineNum = lineNum;
        }

        void wash() {
            auto it = strokes.begin();
            while (it != strokes.end()) {
                if (it->getPointSize() < 1) {
                    it = strokes.erase(it);
                    lineNum--;
                } else {
                    it++;
                }
            }
        }

        void visualize(int thickness = 3) {
            auto getOffsetPoint = [&](const StrokeData::Point &p) -> cv::Point {
                return p + cv::Point(floor(thickness / 2), floor(thickness / 2));
            };
            auto getBoundingSize = [&](int w, int h, int k = 1) -> cv::Size {
                return cv::Size(w, h) * k + cv::Size(thickness, thickness);
            };
            const auto black = cv::Scalar(0), white = cv::Scalar(255);
            cv::Mat image(getBoundingSize(getWidth(), getHeight()),
                          CV_8UC1, white);
            for (const auto &stroke:strokes) {
                const auto &points = stroke.points;
                if (points.size() == 0)continue;
                cv::line(image,
                         getOffsetPoint(points[0]), getOffsetPoint(points[0]),
                         black, thickness, cv::LINE_AA);
                for (auto i = 1; i < points.size(); i++) {
                    cv::line(image,
                             getOffsetPoint(points[i - 1]), getOffsetPoint(points[i]),
                             black, thickness, cv::LINE_AA);
                }
            }
            cv::Mat panel(getBoundingSize(getWidth(), getHeight(), 3),
                          CV_8UC1, white);
            auto rect = cv::Rect(cv::Point(getWidth(), getHeight()),
                                 getBoundingSize(getWidth(), getHeight()));
            image.copyTo(panel(rect));
            cv::rectangle(panel, rect, black, 1);
            cv::destroyAllWindows();
            cv::imshow("display", panel);
//            cv::imshow("display", image);
            cv::waitKey(0);
        }

        bool normalize() {
            auto xmax = numeric_limits<int>::min(),
                    xmin = numeric_limits<int>::max();
            auto ymax = xmax, ymin = xmin;
            for (const auto &stroke:strokes) {
                const auto &points = stroke.points;
                for (auto &point:points) {
                    xmax = xmax > point.x ? xmax : point.x;
                    ymax = ymax > point.y ? ymax : point.y;
                    xmin = xmin < point.x ? xmin : point.x;
                    ymin = ymin < point.y ? ymin : point.y;
                }
            }
            for (auto &stroke:strokes) {
                auto &points = stroke.points;
                for (auto &point:points) {
                    point.x -= xmin;
                    point.y -= ymin;
                }
            }
            setWidth(xmax - xmin);
            setHeight(ymax - ymin);
            if (getWidth() < 0 || getWidth() > 2000 ||
                getHeight() < 0 || getHeight() > 2000) {
                return false;
            }
            cout << *this << endl;
            visualize();
            return true;
        }

        SampleData() : lineNum(0), width(0), height(0) {}

        SampleData(unsigned short int _lineNum) : lineNum(_lineNum) {
            strokes.resize(_lineNum);
        }

        friend inline istream &operator>>(istream &in, SampleData &s) {
//              in>>s.lineNum;
            s.strokes.resize(s.lineNum);
            for (auto &stroke:s.strokes) {
                in >> stroke;
            }
            return in;
        }

        friend inline ostream &operator<<(ostream &out, const SampleData &s) {
            out << "样本\"" << s.label << "\"有" << s.lineNum << "个笔画" << endl;
            for (auto &stroke:s.strokes) {
                out << stroke;
            }
            return out;
        }

        friend inline QDataStream &operator<<(QDataStream &s, const SampleData &o) {
            s << o.height << o.width << o.lineNum << QString(o.label.c_str());
            for (auto &stroke:o.strokes) {
                s << stroke;
            }
            return s;
        }

        friend inline QDataStream &operator>>(QDataStream &s, SampleData &o) {
            QString tmp;
            s >> o.height >> o.width >> o.lineNum >> tmp;
//#ifdef Q_OS_UNIX
//            auto gbk = QTextCodec::codecForName("gb2312");
//            auto utf8 = QTextCodec::codecForName("UTF-8");
//            tmp = gbk->toUnicode(tmp.toStdString().c_str());
//            qDebug() << tmp;
//            QByteArray utf8_bytes = utf8->fromUnicode(tmp);
//            tmp = utf8_bytes.data();
//            qDebug() << tmp;
//#endif
            o.label = tmp.toStdString();
            o.strokes.resize(o.lineNum);
            for (auto &stroke:o.strokes) {
                s >> stroke;
            }
            return s;
        }

    };

    class CouchReader {
    public:
        static CouchReader &GetInstance() {
            static CouchReader cr;
            return cr;
        }

        void washDataSet(vector<SampleData> &src) {
            auto it = src.begin();
            while (it != src.end()) {
                if (it->getStrokeSize() < 1) {// 删除笔迹数量为空的样本
                    it = src.erase(it);
                } else {
                    it->wash();// 删除包含点数为空的笔迹
                    if (it->normalize()) {// 移动到坐标原点，记录边框大小
                        it++;
                    } else { // 边框取值异常
                        it = src.erase(it);
                    }
                }
            }
        }

        void readCouchTopDir(const string &couchTopDir, vector<SampleData> &dst) {
            QDir dir(couchTopDir.c_str());
            dir.setFilter(QDir::Files | QDir::Hidden | QDir::AllDirs);
            QFileInfoList fileInfoList = dir.entryInfoList();
            qDebug() << dir << fileInfoList;
            while (!fileInfoList.isEmpty()) {
                QFileInfo tempFileInfo = fileInfoList.last();
                if (!tempFileInfo.isDir()) {
                    if (tempFileInfo.suffix() == "idx") {
                        readIdxFile(tempFileInfo.absoluteFilePath().toStdString(), dst);
                    }
                    fileInfoList.removeLast();
                } else if (tempFileInfo.fileName() != "." && tempFileInfo.fileName() != "..") {
                    QDir subDir(tempFileInfo.filePath());
                    fileInfoList.removeLast();
                    fileInfoList.append(subDir.entryInfoList());
//                  qDebug() << tempFileInfo;
                } else {
                    fileInfoList.removeLast();
//                  qDebug() << tempFileInfo;
                }
            }
        }

        void readIdxFile(const string &idxFileName, vector<SampleData> &dst) {
            if (idxFileName.find(".idx") == string::npos) {
                cerr << "打开文件失败：" << idxFileName << endl;
                return;
            }
            ifstream idxIfsm(idxFileName, ios::in | ios::binary);
            if (!idxIfsm.is_open()) {
                cerr << "打开文件失败：" << idxFileName << endl;
                return;
            }
//          idxIfsm >> sampleSum;
            idxIfsm.read((char *) &sampleSum, 4);
            string datFileName = idxFileName;
            auto suffixIndex = datFileName.find_last_of(".");
            datFileName.replace(suffixIndex + 1, 3, "dat");
//            cout << datFileName << endl;
            ifstream datIfsm(datFileName, ios::in | ios::binary);
            if (!datIfsm.is_open()) {
                datFileName.replace(suffixIndex + 1, 3, "DAT");
                datIfsm.open(datFileName, ios::in | ios::binary);
                if (!datIfsm.is_open()) {
                    cerr << "打开文件失败：" << datFileName << endl;
                    return;
                }
            }
            while (idxIfsm.read((char *) &sampleState, 1)) {
//              idxIfsm >> oswIndex >> idxIndex >> datOffset;
                idxIfsm.read((char *) &oswIndex, 4);
                idxIfsm.read((char *) &idxIndex, 4);
                idxIfsm.read((char *) &datOffset, 4);
                datIfsm.seekg(datOffset, ios::beg);
//              datIfsm >> wordLength;
                datIfsm.read((char *) &wordLength, 1);
                wordCode = new unsigned char[1 * (size_t) wordLength + 1];
                datIfsm.read((char *) wordCode, 1 * (size_t) wordLength);
                wordCode[1 * (size_t) wordLength] = '\0';
//              datIfsm >> pointNum >> lineNum >> getTimePointNum;
                datIfsm.read((char *) &pointNum, 2);
                datIfsm.read((char *) &lineNum, 2);
                datIfsm.read((char *) &getTimePointNum, 2);
                dst.emplace_back(SampleData(lineNum));
                SampleData &sample = dst.back();
                sample.setLabel((char *) wordCode);
                getTimePointIndex = new unsigned short int[2 * (size_t) getTimePointNum];
                elapsedTime = new int[4 * (size_t) getTimePointNum];
                datIfsm.read((char *) getTimePointIndex, 2 * (size_t) getTimePointNum);
                datIfsm.read((char *) elapsedTime, 4 * (size_t) getTimePointNum);
                datIfsm >> sample;
                freeUselessParm();
//                cout << sample << endl;
//                getchar();
            }
            datIfsm.close();
            idxIfsm.close();
//            cout << "done" << endl;
        }

    private:
        CouchReader() : wordCode(nullptr),
                        getTimePointIndex(nullptr),
                        elapsedTime(nullptr) {}

        ~CouchReader() {}

        CouchReader(const CouchReader &) = delete;

        const CouchReader &operator=(const CouchReader &) = delete;

        void freeUselessParm() {
            if (getTimePointIndex) {
                delete getTimePointIndex;
                getTimePointIndex = nullptr;
            }
            if (elapsedTime) {
                delete elapsedTime;
                elapsedTime = nullptr;
            }
            if (wordCode) {
                delete wordCode;
                wordCode = nullptr;
            }
        }

        long sampleSum;                         //4
        unsigned char sampleState;              //1
        int oswIndex;                           //4
        int idxIndex;                           //4
        int datOffset;                          //4
        unsigned char wordLength;               //1
        unsigned char *wordCode;                //1*WordLength
        unsigned short int pointNum;            //2
        unsigned short int lineNum;             //2
        unsigned short int getTimePointNum;     //2
        unsigned short int *getTimePointIndex;  //2*getTimePointNum
        int *elapsedTime;                       //4*getTimePointNum
    };
}
#endif //_COUCH_H_
