#include <opencv2/opencv.hpp>
#include <iostream>

using namespace std;
using namespace cv;
#include <openbabel/atom.h>
using namespace OpenBabel;
int main(int argc, char **argv) {
    OBAtom abatom;
    Mat img = imread("/home/xgd/Pictures/backup/4.jpg");
    if (img.empty()) {
        cout << "打开图像失败！" << endl;
        return -1;
    }
    imshow("image", img);
    waitKey();

    return 0;
}

