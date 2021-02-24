#include "crnn_data.hpp"
#include "soso_darknet.hpp"
#include "hw_data.hpp"
#include <QCoreApplication>

#include <random>
#include <memory>
#include <iostream>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

extern std::string WORKSPACE;

CRNNDataGenerator crnnDataGenerator;

/**
 * TODO:
 * 1、标准水平长文本
 * 2、细楔形键
 * 3、文本粗细泛化太弱
 * 4、键类型的混淆
 * 5、坐标嵌入
 */
void testYoloDara() {
    srand(0022);
    crnnDataGenerator.init("/tmp/fuck");
    SOSODarknet generator;
    generator.init(WORKSPACE + "soso-obj");
    generator.display();
}

int main(int argc, char **argv) {
    qApp->setAttribute(Qt::AA_EnableHighDpiScaling);
    HwDataLoader::getInstance();
    try {
        testYoloDara();
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        exit(-1);
    }
}
