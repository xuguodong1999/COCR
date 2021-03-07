#include "config.hpp"
#include "crnn_data.hpp"
#include "soso_darknet.hpp"
#include "hw_data.hpp"
#include <QCoreApplication>

#include <random>
#include <memory>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


extern CRNNDataGenerator crnnDataGenerator;

/**
 * TODO:
 * 1、标准水平长文本
 * 2、细楔形键
 * 3、文本粗细泛化太弱
 * 4、键类型的混淆
 * 5、坐标嵌入
 */
void generateYoloData() {
    srand(0022);
    crnnDataGenerator.init(WORKSPACE + "/crnn-text-data");
//    crnnDataGenerator.display();
    SOSODarknet generator;
    generator.init(WORKSPACE + "/soso-obj-data");
    generator.display();
}

#include <QPainter>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>
#include <QLabel>
#include <QApplication>
#include "opencv_util.hpp"


int main(int argc, char **argv) {
//    auto img=cv::imread("/home/xgd/Pictures/2021-02-25 16-59-17 的屏幕截图.png");
//    cv::resize(img,img,cv::Size(img.cols,img.rows)/2);
//    cv::imshow("mm",img);
//    cv::waitKey(0);
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);
    HwDataLoader::getInstance();
    try {
        generateYoloData();
        return 0;//a.exec();
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
