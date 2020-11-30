#include <iostream>
#include <exception>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include "opencv_yolo.hpp"
using namespace std;

int main() {
    try {
        OpenCVYolo yolo;
        yolo.init("C:/Users/xgd/Downloads/soso17-2020-11-30/yolov4-tiny-3l.cfg",
                  "C:/Users/xgd/Downloads/soso17-2020-12-01/yolov4-tiny-3l_92000.weights");
        yolo.forward(cv::Mat(cv::Size(30,30),CV_8UC3));
        cv::Mat img=cv::imread("C:/Users/xgd/Pictures/cocr_test.png");
        yolo.forward(img);
//        system("pause");
        return 0;
    } catch (std::exception &e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }
}